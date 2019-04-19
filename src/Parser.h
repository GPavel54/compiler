#ifndef PARSER_H
#define PARSER_H
#include "addfunctions.h"
#include "Lexer.h"

#define REMOVESYM 888

class Parser
{
    vector<ParsElem> stack;
    stringstream tree;
    int uncover(Token& token);
    int addToStack(string terminalName, string token, string noterminal);
    int getRule(string terminalName, string token, string noterminal);
public:
    void makeSyntaxTree(Lexer & lex);
    Parser();

};

#endif