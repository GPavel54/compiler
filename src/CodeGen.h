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
        int size;
    };
    int sp;
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
    void separateToFunctions(); // разбиение на функции
    void generateAsm();  // основная функция
    void separateFunc(list<Token>& func);  // обработка функций
    void processExpr(Token left, vector<Token>& expression); // обработка выражения
    void translateToRpn(vector<Token>& expression); // перевод переданного выражения в обратную польскую запись
    void createSymbolicTable(list<Token>& func); 
    void createAsm();  // создание ассемблерного файла
    void addVariable(int size, Token& token, string& type, int level, int& bp); // добавление переменной в хэш таблицу
    void getArrayValue(string& name,vector<Token> expression); // возвращает адрес, в который записал требуемое значение

    /* Debug functions */
    void printExpr(vector<Token>& expr);  // выражение
    void printHashTable();
};

#endif