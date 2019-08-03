#ifndef PARSER_H
#define PARSER_H
#include "addfunctions.h"
#include "Lexer.h"

#define REMOVESYM 888
#define EPSILON 777
#define SEPSILON "epsilon"

class Parser
{
    struct treeNode
    {
        string name;
        string token;
        treeNode * nodes;
        int size;
        bool terminal;
        void setEmpty();
        void printTree(treeNode& tn);
    };
    struct toPrintSt
    {
        treeNode * tn;
        int layer;
    };
    vector<ParsElem> stack;
    stringstream tree;
    treeNode syntaxTree;
    vector<treeNode *> products;
    vector<string> commands;

    int uncover(Token& token, int rule);
    int addToStack(string terminalName, string token, string noterminal, int rule);
    int getRule(string terminalName, string token, string noterminal, int rule);
    void addToTree(int rule);
    void addToTree(string terminal);
public:
    void makeSyntaxTree(Lexer & lex);
    void printTree();
    Parser();
};

#endif