#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Using " << argv[0] << " infile.cs outfile" << endl;
        return 1;
    }
    Lexer lex;
    try 
    {
        lex.makeTable(argv[1]);
    }
    catch (Open_exception& ex)
    {
        return ex.what();
    }
    catch (Wseq_exception& ex)
    {
        return ex.what();
    }
    cout << "Table of tokens:" << endl << endl;
    lex.printTable();

    cout << "Making syntax analysis..." << endl;

    Parser par;
    CodeGen cg(lex);
    par.makeSyntaxTree(lex);
    try {
        cg.generateAsm();
    }
    catch (ASMG_exception& ex)
    {
        return ex.what();
    }
}
