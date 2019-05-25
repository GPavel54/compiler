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
    string name;  //token type (e. g. left parathesis)
    string token; //token in program (e. g. myFunc)
};

struct exp
{
    string name;
    regex expression;
};

struct ParsElem
{
    bool terminal;
    Token tok;
    string prod;
};

void eraseFreeSpace(string& str);
void numirateToken(string& token, string& buffer, int offset, int& position);

#endif