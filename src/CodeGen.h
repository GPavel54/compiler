#ifndef CODEGEN_H
#define CODEGEN_H
#include "addfunctions.h"
#include "Lexer.h"
#include <unordered_map>

#define FUNCTION_ 444

class CodeGen
{
    struct Types
    {
        string ident;
        string type;
        int level;
        string address;
    };
    vector<Types> types;
    list<Token> tokens;
    unordered_multimap<string, int> hmap;
    list<list<Token> > functions;
    list<list<Token> > blocks;
    stringstream bss;
    stringstream data;
    stringstream text;
    stringstream asmfile;
public:
    CodeGen(Lexer& lexer);
    void separateToFunctions();
    void generateAsm();
    void separateFunc(list<Token>& func);
    void processExpr(Token left, vector<Token>& expression);
    void translateToRpn(vector<Token>& expression);
    void getVarAddress(Token& var, string& address);
};

#endif