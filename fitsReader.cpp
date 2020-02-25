#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <iomanip>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>

/*
A Structure to store 80 character headers.
*/
struct fits
{
    int position;        //position of the header in the file.
    std::string keyword; //header name
    std::string value;   //header value
};

/*
What it can do?
1. Read FITS File and extract headers.
2. Write a particular header into file.
Limitaions:
1. Only 20 character integer or float.
2. Comment may be distoted after modification.
3. Can't modify comments.
4. Ignores 'COMMENT' and 'HISTORY' keywords.
5. Can't read extensions if any.
*/
class readfits
{
    std::map<std::string, fits> token;        /*Map to store keywords their value and position */
    void extract_headers(std::fstream &file); /*Function for read headers*/
    std::string file_name;

public:
    readfits(std::string name)
    {
        file_name = name;
        std::fstream file(name, std::ios::in | std::ios::binary);
        extract_headers(file);
    }

    void update_headers(std::string); /*Function for modify headers*/
    void display(std::string);        /*Function for displaying headers*/
};

/*
Arguments: 'file' -> File object corresponding to file name.
Extracts the headers from fits file and stores in map.
*/
void readfits::extract_headers(std::fstream &file)
{
    fits temp;
    char cutset[80];
    token.clear();
    while (true)
    {
        temp.position = file.tellp();
        file.read(cutset, 80);
        // read 80 character
        std::string word(cutset);

        /*
Ignoring HISTORY and COMMENT keywords for simplicity.
*/
        if (word.at(0) == '/' ||
            !word.substr(0, 7).compare("HISTORY") ||
            !word.substr(0, 7).compare("COMMENT"))
        {
            continue;
        }

        std::string key_ = word.substr(0, 8), second = word.substr(10, 70); /*Seperates keyword and value*/
        //Trim Whitespaces
        boost::algorithm::trim(key_);
        boost::algorithm::trim(second);
        temp.keyword = key_;
        temp.value = second;

        token.emplace(key_, temp); //Store into map

        if (!word.substr(0, 3).compare("END")) //Stop reading condition
        {
            file.close();
            break;
        }
    }
}

/*
Arguments: 'key' -> the keyword to modify
Modifies the current value of the header
*/
void readfits::update_headers(std::string key)
{
    std::fstream writer(file_name, std::ios::in | std::ofstream::binary | std::ios::out);
    fits refer = token[key];
    std::string update_string;
    std::string replace;
    replace = key;
    int token_length = replace.length();

    /*
    Reformatting of the update string according to the FITS standards.
    */
replace_routine:
{
    std::getline(std::cin, update_string, '\n');

    while (token_length < 8) /*turns 'KEYWORD' into 'KEYWORD = ' 10 character string*/
    {
        replace += " ";
        token_length++;
    }
    replace += "= ";
    token_length += 2;

    /*
Checks if value is logical E.g. 'T' or 'F'
*/
    if (!(refer.value.compare("T")) || !(refer.value.compare("F")))
    {
        if (update_string.length() != 1)
        {
            std::cerr << "Please enter a valid value" << std::endl;
            std::getline(std::cin, update_string, '\n');
        }
        while (token_length < 29)
        {
            replace += " ";
            token_length++;
        }
        replace += update_string;
        token_length++;
    }
    else if (refer.value.at(0) == '\'') /*Or if the value is character string*/
    {
        replace += "'";
        token_length++;
        if (update_string.length() > 67)
        {
            std::cerr << "Enter a valid value" << std::endl;
            std::getline(std::cin, update_string, '\n');
        }
        replace += update_string + "'";
        token_length += update_string.length() + 1;
    }
    else /*Or the value is numeric. Integer, float or complex*/
    {
        if (update_string.length() > 20)
        {
            std::cerr << "Enter a valid value" << std::endl;
            std::cin >> update_string;
        }
        replace += update_string;
        token_length += update_string.length();
        while (token_length < 30)
        {
            replace.insert(10, " ");
            token_length++;
        }
    }
    replace += " ";
    token_length++;
}

    /*
Append the previous comment if any
*/
    int comm_len = refer.value.length();
    char limit = refer.value.at(comm_len - 1);
    while (limit != '/')
    {
        comm_len--;
        limit = refer.value.at(comm_len);
    }
    std::string app_comment = refer.value.substr(comm_len - 1, refer.value.length());
    boost::trim(app_comment);
    replace += app_comment;
    token_length += app_comment.length();

    /*
    Check if new token is of correct length or not. 
    */
    if (token_length > 80)
    {
        std::cerr << "Some error occored, please enter again" << std::endl;
        replace = replace.substr(0, 80);
    }
    else
    {
        while (token_length < 80)
        {
            replace += " ";
            token_length++;
        }
    }
    replace += "\0"; //Terminates the update string

    std::cout << "Current value is :" << std::endl;
    display(key);
    /*
    Seeks to the file postion of the keyword and then writes 
    80 characters.
    */
    writer.seekp(refer.position);
    writer.write(replace.c_str(), 80);
    writer.close();

    /*
    Reruns the extract header routine to update the value in the map.
    */
    std::fstream updated(file_name, std::ios::in | std::ios::binary);
    extract_headers(updated);
    std::cout << "Updated value is :" << std::endl;
    display(key);
}

/*
Arguments: 'command' -> the keyword to be displayed.
Display the value extracted from the file.
*/
void readfits::display(std::string command)
{
    if (!command.compare("ALL"))
    {
        for (auto &&par : token)
        {
            std::cout << std::setw(10) << par.first
                      << std::setw(75) << par.second.value
                      << std::setw(10) << par.second.position << std::endl;
        }
    }
    else
    {
        std::cout << std::setw(10) << command << std::setw(75) << token[command].value << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    std::string to_read;
    std::cout << "Enter file name with extension e.g. 'file.fits'" << std::endl;
    std::cin >> to_read;
    readfits check(to_read);

    /*
Creates a map of functions to execute the commands like 'display' and 'update'.
*/
    std::map<std::string, std::function<void(std::string)>> instruction;
    instruction["update"] = [&check](std::string in) { check.update_headers(in); };
    instruction["display"] = [&check](std::string in) { check.display(in); };

    std::cout << "Enter the command followed by arguments, enter 'end' to exit" << std::endl;
    std::string command, argument;
    std::cin >> command;

    while (command.compare("end"))
    {
        std::cin >> argument;
        boost::to_upper(argument);
        instruction[command](argument);
        std::cin >> command;
    }
    return 0;
}
