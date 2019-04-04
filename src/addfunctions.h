#ifndef FUNC_H
#define FUNC_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <map>

using namespace std;

struct Location
{
    int line;
    int symb;
};

struct exp
{
    string name;
    regex expression;
};

void eraseFreeSpace(string& str);

#endif