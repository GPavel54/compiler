#ifndef PARSER_H
#define PARSER_H
#include "addfunctions.h"
#include "Lexer.h"

#define REMOVESYM 888

class Parser
{
    vector<ParsElem> stack;
    stringstream tree;
    int uncover(Token& token, int rule);
    int addToStack(string terminalName, string token, string noterminal, int rule);
    int getRule(string terminalName, string token, string noterminal, int rule);
public:
    void makeSyntaxTree(Lexer & lex);
    Parser();

};

#endif