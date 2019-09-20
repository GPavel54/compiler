#ifndef IR_H
#define IR_H

#include "addfunctions.h"
#include "Lexer.h"
#include <unordered_map>
#include <sstream>

#define FUNCTION_ 444

class IR
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
        string toPaste;
    };
    struct BreakNum
    {
        bool needBreak;
        int num;
    };
    struct VarInfo
    {
        string name;
        int size;
        string type;
        int block;
    };
    int sp;
    int reserved_memory, memory_counter;
    int t;

    bool remember;
    string remembered_variable1;
    string remembered_variable2;

    vector<VarInfo > symbolicTable;
    vector<BlockInfo> blocksType;
    BlockInfo nextBlock;
    BreakNum nbreak;
    list<Token> tokens;
    unordered_multimap<string, Info> hmap;
    list<list<Token> > functions;
    list<list<Token> > blocks;
    stringstream text;
    int block_number;
public:
    enum TYPE{
        CHAR,
        INT,
        STRING
    };
    IR(Lexer& lexer);
    void separateToFunctions(); // разбиение на функции
    void generateIR();  // основная функция
    void separateFunc(list<Token>& func);  // обработка функций
    void processExpr(Token left, vector<Token>& expression, int shift = 0); // обработка выражения
    void translateToRpn(vector<Token>& expression); // перевод переданного выражения в обратную польскую запись
    void createSymbolicTable(list<Token>& func); 
    void addVariable(int size, Token& token, string& type, int level, int& bp); // добавление переменной в хэш таблицу
    void getArrayValue(string& name,vector<Token> expression); // помещает на вершину стэка требуемое значение

    /* Еще не до конца протестированные функции */
    void printVariable(TYPE type, string address = ""); // распечатывает значение, которое находится на вершине стека

    /* Debug functions */
    void printExpr(vector<Token>& expr);  // выражение
    void printHashTable();
};

#endif