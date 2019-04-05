#ifndef LEXER_H
#define LEXER_H
#include "addfunctions.h"

class Lexer
{
public:
    void initializeMap();
    void getType(string& word, string& type);
    void split(const string &s, char delim, vector<string> &elems);
    Lexer(string path);

    vector<Token> tokens_;
    vector<exp> expressions_;
};

#endif