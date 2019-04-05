#include "Lexer.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Using " << argv[0] << " infile.cs outfile" << endl;
        return 1;
    }
    try 
    {
        Lexer lex(argv[1]);
    }
    catch (Open_exception& ex)
    {
        return ex.what();
    }
    catch (Wseq_exception& ex)
    {
        return ex.what();
    }
}
