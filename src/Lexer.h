#ifndef LEXER_H
#define LEXER_H
#include "addfunctions.h"

class Lexer
{
public:
    Lexer();
    void initializeMap();
    void getType(string& word, string& type);
    void split(const string &s, char delim, vector<string> &elems);
    void makeTable(string path);

    vector<Token> tokens_;
    vector<exp> expressions_;
};

#endif