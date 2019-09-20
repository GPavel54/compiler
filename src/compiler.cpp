#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "IR.h"

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
    // cout << "Table of tokens:" << endl << endl;
    // lex.printTable();

    Parser par;
    IR ir(lex);
    CodeGen cg(lex);
    try
    {
        ir.generateIR();
    }
    catch (ASMG_exception& ex)
    {
        return ex.what();
    }
    try 
    {
        par.makeSyntaxTree(lex);
    }
    catch (ASMG_exception& ex)
    {
        return ex.what();
    }
    try 
    {
        cg.generateAsm(argv[2]);
    }
    catch (ASMG_exception& ex)
    {
        return ex.what();
    }
    catch (Open_exception& ex)
    {   
        return ex.what();
    }
}
