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
    readfits(){};
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