#ifndef CODEGEN_H
#define CODEGEN_H
#include "addfunctions.h"
#include "Lexer.h"
#include <unordered_map>

#define FUNCTION_ 444

class CodeGen
{
    struct Info
    {
        // string ident;
        string type;
        int level;
        string address; // адрес это смещение от значения в регистре rbp
    };
    vector<vector<string> > symbolicTable;
    list<Token> tokens;
    unordered_multimap<string, Info> hmap;
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
    void createSymbolicTable(list<Token>& func);
    void createAsm();
    void addVariable(int size, Token& token, string& type, int level, int& bp);

    /* Debug functions */
    void printExpr(vector<Token>& expr);
};

#endif