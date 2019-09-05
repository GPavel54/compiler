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
    struct BlockInfo
    {
        string type;
        int num;
    };
    int sp;
    int reserved_memory, memory_counter;
    vector<vector<string> > symbolicTable;
    vector<BlockInfo> blocksType;
    BlockInfo nextBlock;
    list<Token> tokens;
    unordered_multimap<string, Info> hmap;
    list<list<Token> > functions;
    list<list<Token> > blocks;
    stringstream bss;
    stringstream data;
    stringstream text;
    stringstream asmfile;
public:
    enum TYPE{
        CHAR,
        INT,
        STRING
    };
    CodeGen(Lexer& lexer);
    void separateToFunctions(); // разбиение на функции
    void generateAsm(string path);  // основная функция
    void separateFunc(list<Token>& func);  // обработка функций
    void processExpr(Token left, vector<Token>& expression, int shift = 0); // обработка выражения
    void translateToRpn(vector<Token>& expression); // перевод переданного выражения в обратную польскую запись
    void createSymbolicTable(list<Token>& func); 
    void createAsm();  // создание ассемблерного файла
    void addVariable(int size, Token& token, string& type, int level, int& bp); // добавление переменной в хэш таблицу
    void getArrayValue(string& name,vector<Token> expression); // помещает на вершину стэка требуемое значение

    /* Еще не до конца протестированные функции */
    void printVariable(TYPE type, string address = ""); // распечатывает значение, которое находится на вершине стека

    /* Debug functions */
    void printExpr(vector<Token>& expr);  // выражение
    void printHashTable();
};

#endif