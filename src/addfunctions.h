#ifndef FUNC_H
#define FUNC_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <regex>
#include <map>
#include <iomanip> 
#include <exception>
#include "Errors.h"

using namespace std;

struct Token
{
    int row;
    int col;
    string name;
    string token;
};

struct exp
{
    string name;
    regex expression;
};

void eraseFreeSpace(string& str);
void numirateToken(string& token, string& buffer, int offset, int& position);

#endif