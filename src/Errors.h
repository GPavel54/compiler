#ifndef ERRORS_H
#define ERRORS_H
#include "addfunctions.h"

using namespace std;

class Open_exception: public std::exception
{
    public:
    int what()
    {
        cerr << "Cant open file" << endl;
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
        cerr << "Unknown sequence " << seq << " at line " << line << endl;
        cerr << "Closing file ..." << endl;
        return 1;
    }
};

#endif