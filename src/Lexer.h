#ifndef LEXER_H
#define LEXER_H
#include "addfunctions.h"

class Lexer
{
    vector<exp> expressions;
public:
    void initializeMap();
    void getType(string& word, string& type);
    void split(const string &s, char delim, vector<string> &elems);
};

#endif