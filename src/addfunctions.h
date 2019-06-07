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

struct expre
{
    string name;
    regex expression;
};

struct Token
{
    int row;
    int col;
    string name;  //token type (e. g. left parathesis)
    string token; //token in program (e. g. myFunc)
};

struct ParsElem
{
    bool terminal;
    Token tok;
    string prod;
};

void eraseFreeSpace(string& str);
void numirateToken(string& token, string& buffer, int offset, int& position);
void checkStringLiterals(vector<string>& buffer);

#endif