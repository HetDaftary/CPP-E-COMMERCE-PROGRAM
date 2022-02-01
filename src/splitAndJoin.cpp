#include "../include/splitAndJoin.hpp"

using std::string;
using std::vector;

string join(string delim, vector<string> strings) {
    int size = strings.size() - 1;
    int sizeToReturn = size * delim.size();

    int toStartAt[size + 1];
    toStartAt[0] = 0;

    for (int i = 0; i < size; i++) {
        sizeToReturn += strings[i].size();
        toStartAt[i + 1] = toStartAt[i] + strings[i].size() + delim.size();
    } sizeToReturn += strings[size].size();

    char* result = new char[sizeToReturn + 1];

    for (int i = 0; i < size; i++) {
        strcpy(result + toStartAt[i], strings[i].c_str());
        strcpy(result + toStartAt[i] + strings[i].size(), delim.c_str());  
    }

    strcpy(result + toStartAt[size], strings[size].c_str());

    return string(result);
}

vector<string> split(string delim, string toSplit) {
    char* toSplitC = (char*)toSplit.c_str();
    
    vector<string> result;

    char *token = strtok(toSplitC, delim.c_str());
    while (token) {
        result.push_back(string(token));
        token = strtok(NULL, delim.c_str());
    }

    return result;
}