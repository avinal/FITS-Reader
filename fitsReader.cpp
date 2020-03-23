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
#include <iterator>
#include <cstdlib>

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
3. Can't read extensions if any.
*/
class readfits
{
    std::multimap<std::string, fits> token; /*Map to store keywords their value and position */
    std::multimap<std::string, fits> xtoken;
    void extract_headers(std::fstream &file);   /*Function for read headers*/
    void extract_extension(std::fstream &file); /*Function to read Extensions*/
    void write_headers(std::string key, std::string replace, int position, int token_len);
    std::string file_name;

public:
    readfits(std::string name)
    {
        file_name = name;
        std::fstream file(name, std::ios::in | std::ios::binary);
        extract_headers(file);
        extract_extension(file);
    }

    void update_headers(std::string); /*Function for modify headers*/
    void display(std::string);        /*Function for displaying headers*/
};

/*
Arguments: 'file' -> File object corresponding to file name.
Extracts the headers from fits file and stores in multimap.
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

        std::string key_, second; /*Seperates keyword and value*/

        key_ = word.substr(0, 8);

        //Trim Whitespaces
        boost::algorithm::trim(key_);
        if (!key_.compare("HISTORY") || !key_.compare("COMMENT"))
        {
            second = word.substr(8, 72);
        }
        else
        {
            second = word.substr(10, 70);
        }
        boost::algorithm::trim(second);
        temp.keyword = key_;
        temp.value = second;

        token.emplace(key_, temp); //Store into multimap

        if (!word.substr(0, 3).compare("END")) //Stop reading condition
        {
            break;
        }
    }
}

void readfits::extract_extension(std::fstream &file)
{
    fits end_tok = token.find("END")->second;
    int end_pos = end_tok.position;
    end_tok = token.find("NAXIS")->second;
    std::string naxistr = end_tok.value;
    int naxis = boost::lexical_cast<int>(naxistr.substr(0, naxistr.find(" ")));
    int naxisn[naxis];
    long dat = 1;
    for (int i = 1; i <= naxis; i++)
    {
        std::string nstr = "NAXIS" + std::to_string(i);
        end_tok = token.find(nstr)->second;
        std::string naxisnstr = end_tok.value;
        naxisn[i - 1] = boost::lexical_cast<int>(naxisnstr.substr(0, naxisnstr.find(" ")));
        dat *= naxisn[i - 1];
    }
    end_tok = token.find("BITPIX")->second;
    std::string bitstr = end_tok.value;
    int bitpix = boost::lexical_cast<int>(bitstr.substr(0, bitstr.find(" ")));
    dat *= (abs(bitpix) / 8);
    end_pos = (end_pos % 2880 == 0) ? end_pos / 2880 : end_pos / 2880 + 1;
    dat = (dat % 2880 == 0) ? dat / 2880 : dat / 2880 + 1;
    int xtn_pos = 2880 * (end_pos + dat);
    file.seekg(0, std::ios::end);
    if (xtn_pos >= file.tellg())
    {
        std::cout << "Reached end of file. No Extensions !!";
        file.close();
        return;
    }
    file.seekg(xtn_pos);
    fits temp;
    char cutset[80];
    xtoken.clear();
    while (true)
    {
        temp.position = file.tellp();
        file.read(cutset, 80);
        // read 80 character
        std::string word(cutset);

        std::string key_, second; /*Seperates keyword and value*/

        key_ = word.substr(0, 8);

        //Trim Whitespaces
        boost::algorithm::trim(key_);
        if (!key_.compare("HISTORY") || !key_.compare("COMMENT"))
        {
            second = word.substr(8, 72);
        }
        else
        {
            second = word.substr(10, 70);
        }
        boost::algorithm::trim(second);
        temp.keyword = key_;
        temp.value = second;

        xtoken.emplace(key_, temp); //Store into multimap

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
    std::string update_string;
    std::getline(std::cin, update_string, '\n');
    auto token_it = token.equal_range(key);
    fits refer = token_it.first->second;
    int update_position = refer.position;
    std::string replace;
    replace = key;
    int token_length = replace.length();
    if (!key.compare("COMMENT") || !key.compare("HISTORY"))
    {
        display(key);
        std::cout << "Enter the position of the header you want to change" << std::endl;
        std::cin >> update_position;
        replace += " ";
        replace += update_string;
        token_length += update_string.length() + 1;
        write_headers(key, replace, update_position, token_length);
        return;
    }
    else if (!key.compare("SIMPLE") ||
             !key.compare("BITPIX") ||
             !key.compare("NAXIS") ||
             !key.compare("END"))
    {
        std::cerr << "Mandatory keywords cannot be modified !!" << std::endl;
        return;
    }

    /*
    Reformatting of the update string according to the FITS standards.
    */
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

    /*
Append the previous comment if any
*/
    int comm_len = refer.value.length();
    char limit = refer.value.at(comm_len - 1);
    while (limit != '/' && comm_len > 0)
    {
        comm_len--;
        limit = refer.value.at(comm_len);
    }
    std::string app_comment;
    if (!comm_len)
    {
        app_comment = "";
    }
    else
    {
        app_comment = refer.value.substr(comm_len - 1, refer.value.length());
    }
    boost::trim(app_comment);
    replace += app_comment;
    token_length += app_comment.length();
    write_headers(key, replace, update_position, token_length);
}

void readfits::write_headers(std::string key,
                             std::string replace,
                             int position,
                             int token_len)
{
    std::fstream writer(file_name, std::ios::in | std::ofstream::binary | std::ios::out);
    /*
    Check if new token is of correct length or not. 
    */
    if (token_len > 80)
    {
        std::cerr << "Some error occored, please enter again" << std::endl;
        replace = replace.substr(0, 80);
    }
    else
    {
        replace.append(80 - token_len, ' ');
        token_len = replace.length();
    }
    replace += "\0"; //Terminates the update string

    std::cout << "Current value is :" << std::endl;
    display(key);
    /*
    Seeks to the file postion of the keyword and then writes 
    80 characters.
    */
    writer.seekp(position);
    writer.write(replace.c_str(), 80);
    writer.close();

    /*
    Reruns the extract header routine to update the value in the multimap.
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
        std::cout << std::endl
                  << "extension\n";
        for (auto &&xpar : xtoken)
        {
            std::cout << std::setw(10) << xpar.first
                      << std::setw(75) << xpar.second.value
                      << std::setw(10) << xpar.second.position << std::endl;
        }
    }
    else if (!command.compare("XTENSION"))
    {
        for (auto &&xpar : xtoken)
        {
            std::cout << std::setw(10) << xpar.first
                      << std::setw(75) << xpar.second.value
                      << std::setw(10) << xpar.second.position << std::endl;
        }
    }
    else
    {
        auto token_range = token.equal_range(command);

        if (!command.compare("COMMENT") || !command.compare("HISTORY"))
        {
            std::cout << "There are multiple entries for the given keywords." << std::endl;
        }

        for (auto it = token_range.first; it != token_range.second; it++)
        {
            std::cout << std::setw(10) << it->first
                      << std::setw(75) << it->second.value
                      << std::setw(10) << it->second.position << std::endl;
        }
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
