#ifndef ERRORS_H
#define ERRORS_H
#include "addfunctions.h"

using namespace std;

const static string PREFIX = "\033[1;31mError:\033[0;0m ";

class Open_exception: public std::exception
{
public:
    int what()
    {
        cerr << PREFIX << "Cant open file" << endl;
        return 1;
    }
};

class Wseq_exception: public std::exception
{
    string seq;
    int line;
public:
    Wseq_exception(string s, int l, ifstream& stream)
    {
        seq = s;
        line = l;
        stream.close();
    }
    int what()
    {
        cerr << PREFIX << "Unknown sequence " << seq << " at line " << line << endl;
        cerr << "Closing file ..." << endl;
        return 1;
    }
};

class ASMG_exception : public std::exception
{
    protected:
    string ident;
    int row;
    int col;
    string message;
    public:
    ASMG_exception(string& n, int r, int c)
    {
        ident = n;
        row = r;
        col = c;
    }
    int what()
    {
        cerr << PREFIX << message << "\e[1;36m" << ident << "\e[0;0m row = "
            << row << " col = " << col << endl;
        return 1;
    }
};

class Syntax_exception: public ASMG_exception
{
public:
    Syntax_exception(string& n, int r, int c) : ASMG_exception (n, r, c) 
    {
        message = "Syntax error in: ";
    }
};

class Mdefinition_exception: public ASMG_exception
{
public:
    Mdefinition_exception(string& n, int r, int c) : ASMG_exception (n, r, c) 
    {
        message = "Multiply definition of identificator:";
    }
};

class Ndefined_exception: public ASMG_exception
{
public:
    Ndefined_exception(string& n, int r, int c) : ASMG_exception (n, r, c) 
    {
        message = "The identifier didn't define: ";
    }
};

class StaticSize_exception: public ASMG_exception
{
public:
    StaticSize_exception(string& n, int r, int c) : ASMG_exception (n, r, c) 
    {
        message = "Array size must be a constatnt: ";
    }
};

class ArrayInit_exception: public ASMG_exception
{
public:
    ArrayInit_exception(string& n, int r, int c) : ASMG_exception (n, r, c) 
    {
        message = "Do not initialize integer array: ";
    }
};

#endif