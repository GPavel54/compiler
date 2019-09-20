#ifndef LEXER_H
#define LEXER_H
#include "addfunctions.h"

class Lexer
{
    list<Token> tokens_;
    vector<expre> expressions_;
    friend class Parser;
    friend class CodeGen;
    friend class IR;
public:
    Lexer();
    void initializeMap();
    void getType(string& word, string& type);
    void split(const string &s, char delim, vector<string> &elems);
    void makeTable(string path);
    void printTable();
};

#endif