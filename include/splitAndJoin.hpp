#ifndef _SPLIT_AND_JOIN_HPP
#define _SPLIT_AND_JOIN_HPP

#include <vector>
#include <cstring>
#include <iostream>

/**
 * @brief Joins n strings with Delim.
 * 
 * @param delim: The delimiter to be used.
 * @param strings: The strings to be joined.
 * @return string: The joined string.
 */
std::string join(std::string delim, std::vector<std::string> strings); 

/**
 * @brief Splits the string based on delim.
 * 
 * @param delim: The delimiter/token to be used.
 * @param toSplit: The string to be split.
 * @return vector<string> : The strings separateed around the token.
 */
std::vector<std::string> split(std::string delim, std::string toSplit);
#endif