#include "Parser.h"

Parser::Parser()
{
    ParsElem tmp;
    tmp.terminal = false;
    tmp.prod = "S";
    stack.push_back(tmp);
}

void Parser::makeSyntaxTree(Lexer& lex)
{
    int ret = 0;
    while (1)
    { 
        if (!lex.tokens_.empty())
        {
            ret = uncover(lex.tokens_.front());
            if (ret == REMOVESYM)
            {
                lex.tokens_.pop_front();
            }
            else if (ret == 0)
            {
                break;
            }
            {
                tree << " → " << ret;
            }
        }   
        else
        {
            break;
        }
    }
    if (ret == 0)
    {
        cout << "Error int your language in col = " << lex.tokens_.front().col << " row = " << lex.tokens_.front().col << endl;
    }
    else
    {
        cout << "Tree : " << endl << tree.str() << endl;
    }
}

int Parser::uncover(Token& token)
{
    int ret = 0;
    if (!stack.back().terminal)
    {
        ret = addToStack(token.name, token.token, stack.back().prod);
        if (ret == 0)
        {
            return ret;
        }
    }
    else
    {
        if (stack.back().tok.name == token.name)
        {
            stack.pop_back();
            return REMOVESYM;
        }
    }
    return ret;
}

int Parser::addToStack(string terminalName, string token, string noterminal)
{
    int ret = getRule(terminalName, token, noterminal);
    ParsElem tmp;
    if (ret == 0)
    {
        return ret;
    }
    stack.pop_back();
    switch (ret)
    {
        case 1:
            tmp.terminal = true;
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Function";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left brace";
            stack.push_back(tmp);
            tmp.tok.name = "Right parenthesis";
            stack.push_back(tmp);
            tmp.tok.name = "Left parenthesis";
            stack.push_back(tmp);
            tmp.tok.name = "main";
            stack.push_back(tmp);
            tmp.tok.name = "Type";
            stack.push_back(tmp);
            tmp.tok.name = "Left brace";
            stack.push_back(tmp);
            tmp.tok.name = "Identifier";
            stack.push_back(tmp);
            tmp.tok.name = "class";
            stack.push_back(tmp);
            break;
        case 2:
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.prod = "Count";
            stack.push_back(tmp);
            break;
        case 3:
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.prod = "Loop";
            stack.push_back(tmp);
            break;
        case 4:
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.prod = "Condition";
            stack.push_back(tmp);
            break;
        case 5:
            tmp.terminal = true;
            tmp.tok.name = "Semi";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "M";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "return";
            stack.push_back(tmp);
            break;
        case 6:
            break;
        case 7:
            tmp.terminal = false;
            tmp.prod = "Init";
            stack.push_back(tmp);
            break;
        case 8:
            tmp.terminal = false;
            tmp.prod = "Expr";
            stack.push_back(tmp);
            tmp.prod = "Ident";
            stack.push_back(tmp);
            break;
        case 9:
            tmp.terminal = true;
            tmp.tok.name = "Semi";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Formula";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Operator =";
            stack.push_back(tmp);
            break;
        case 10:
            tmp.terminal = false;
            tmp.prod = "B";
            stack.push_back(tmp);
            tmp.prod = "A";
            stack.push_back(tmp);
            break;
        case 11:
            tmp.terminal = false;
            tmp.prod = "B";
            stack.push_back(tmp);
            tmp.prod = "A";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Operator 2";
            stack.push_back(tmp);
            break;
        case 12:
            break;
        case 13:
            tmp.terminal = false;
            tmp.prod = "D";
            stack.push_back(tmp);
            tmp.prod = "C";
            stack.push_back(tmp);
            break;
        case 14:
            tmp.terminal = false;
            tmp.prod = "D";
            stack.push_back(tmp);
            tmp.prod = "C";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Operator 1";
            stack.push_back(tmp);
            break;
        case 15:
            break;
        case 16:
            tmp.terminal = true;
            tmp.tok.name = "Right parenthesis";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Formula";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left parenthesis";
            stack.push_back(tmp);
            break;
        case 17:
            tmp.terminal = false;
            tmp.prod = "Number";
            stack.push_back(tmp);
            break;
        case 18:
            tmp.terminal = false;
            tmp.prod = "Ident";
            stack.push_back(tmp);
            break;
        case 19:
            tmp.terminal = true;
            tmp.tok.name = "Constant";
            stack.push_back(tmp);
            break;
        case 20:
            tmp.terminal = true;
            tmp.tok.name = "Constant";
            stack.push_back(tmp);
            tmp.tok.name = "Operator 2";
            stack.push_back(tmp);
            break;
        case 21:
            tmp.terminal = false;
            tmp.prod = "E";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left brace";
            stack.push_back(tmp);
            tmp.tok.name = "Right parenthesis";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Logical";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left parenthesis";
            stack.push_back(tmp);
            tmp.tok.name = "if";
            stack.push_back(tmp);
            break;
        case 22:
            tmp.terminal = true;
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left brace";
            stack.push_back(tmp);
            tmp.tok.name = "else";
            stack.push_back(tmp);
            break;
        case 23:
            break;
        case 24:
            tmp.terminal = false;
            tmp.prod = "F";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Logical operator";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Ident";
            stack.push_back(tmp);
            break;
        case 25:
            tmp.terminal = false;
            tmp.prod = "Ident";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Logical operator";
            stack.push_back(tmp);
            tmp.tok.name = "Constant";
            stack.push_back(tmp);
            break;
        case 26:
            tmp.terminal = false;
            tmp.prod = "Ident";
            stack.push_back(tmp);
            break;
        case 27:
            tmp.terminal = true;
            tmp.tok.name = "Constant";
            stack.push_back(tmp);
            break;
        case 28:
            tmp.terminal = true;
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left brace";
            stack.push_back(tmp);
            tmp.tok.name = "Right parenthesis";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Logical";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left parenthesis";
            stack.push_back(tmp);
            tmp.tok.name = "while";
            stack.push_back(tmp);
            break;
        case 29:
            tmp.terminal = false;
            tmp.prod = "L";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Right parenthesis";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left parenthesis";
            stack.push_back(tmp);
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Param";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Ident";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Type";
            stack.push_back(tmp);
            break;
        case 30:
            tmp.terminal = false;
            tmp.prod = "Function";
            stack.push_back(tmp);
            break;
        case 31:
            break;
        case 32:
            tmp.terminal = false;
            tmp.prod = "G";
            stack.push_back(tmp);
            tmp.prod = "Ident";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Type";
            stack.push_back(tmp);
            break;
        case 33:
            tmp.terminal = false;
            tmp.prod = "G";
            stack.push_back(tmp);
            tmp.prod = "Ident";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Type";
            stack.push_back(tmp);
            tmp.tok.name = ",";
            stack.push_back(tmp);
            break;
        case 34:
            break;
        case 35:
            tmp.terminal = false;
            tmp.prod = "H";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Type";
            stack.push_back(tmp);
            break;
        case 36:
            tmp.terminal = false;
            tmp.prod = "N";
            stack.push_back(tmp);
            tmp.prod = "Ident";
            stack.push_back(tmp);
            break;
        case 37:
            tmp.terminal = true;
            tmp.tok.name = "Semi";
            stack.push_back(tmp);
            break;
        case 38:
            tmp.terminal = false;
            tmp.prod = "Expr";
            stack.push_back(tmp);
            break;
        case 39:
            tmp.terminal = false;
            tmp.prod = "J";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Identifier";
            stack.push_back(tmp);
            break;
        case 40:
            tmp.terminal = true;
            tmp.tok.name = "Right index";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "K";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left index";
            stack.push_back(tmp);
            break;
        case 41:
            break;
        case 42:
            tmp.terminal = true;
            tmp.tok.name = "Constant";
            stack.push_back(tmp);
            break;
        case 43:
            tmp.terminal = false;
            tmp.prod = "Ident";
            stack.push_back(tmp);
            break;
        case 44:
            tmp.terminal = false;
            tmp.prod = "Number";
            stack.push_back(tmp);
            break;
        case 45:
        case 46:
            break;
    }
    return ret;
}

int Parser::getRule(string terminalName, string token, string noterminal)
{
    if (noterminal == "S")
    {
        if (terminalName == "Class")
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Prog")
    {
        if (terminalName == "Identifier")
        {
            return 2;
        }
        else if (terminalName == "if")
        {
            return 4;
        }
        else if (terminalName == "while")
        {
            return 3;
        }
        else if (terminalName == "Type")
        {
            return 2;
        }
        else if (terminalName == "return")
        {
            return 5;
        }
        else
        {
            return 6;
        }
    }
    if (noterminal == "Count")
    {
        if (terminalName == "Identifier")
        {
            return 8;
        }
        else if (terminalName == "Type")
        {
            return 7;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Expr")
    {
        if (terminalName == "Operator =")
        {
            return 9;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Formula")
    {
        if (terminalName == "Identifier")
        {
            return 10;
        }
        else if (terminalName == "Constant")
        {
            return 10;
        }
        else if (terminalName == "Left parenthesis")
        {
            return 10;
        }
        else if (terminalName == "Operator 2" && token == "-")
        {
            return 10;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "A")
    {
        if (terminalName == "Identifier")
        {
            return 13;
        }
        else if (terminalName == "Constant")
        {
            return 13;
        }
        else if (terminalName == "Left parenthesis")
        {
            return 13;
        }
        else if (terminalName == "Operator 2" && token == "-")
        {
            return 13;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "B")
    {
        if (terminalName == "Operator 2")
        {
            return 11;
        }
        else
        {
            return 12;
        }
    }
    if (noterminal == "D")
    {
        if (terminalName == "Operator 1")
        {
            return 14;
        }
        else
        {
            return 15;
        }
    }
    if (noterminal == "C")
    {
        if (terminalName == "Identifier")
        {
            return 17;
        }
        else if (terminalName == "Constant")
        {
            return 17;
        }
        else if (terminalName == "Left parenthesis")
        {
            return 16;
        }
        else if (terminalName == "Operator 2" && token == "-")
        {
            return 17;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Number")
    {
        if (terminalName == "Identifier")
        {
            return 18;
        }
        else if (terminalName == "Constant")
        {
            return 19;
        }
        else if (terminalName == "Operator 2" && token == "-")
        {
            return 20;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Condition")
    {
        if (terminalName == "if")
        {
            return 21;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "E")
    {
        if (terminalName == "else")
        {
            return 22;
        }
        else
        {
            return 23;
        }
    }
    if (noterminal == "Logical")
    {
        if (terminalName == "Identifier")
        {
            return 24;
        }
        else if (terminalName == "Constant")
        {
            return 25;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "F")
    {
        if (terminalName == "Identifier")
        {
            return 26;
        }
        else if (terminalName == "Constant")
        {
            return 27;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Loop")
    {
        if (terminalName == "while")
        {
            return 28;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Function")
    {
        if (terminalName == "Type")
        {
            return 29;
        }
        else
        {
            return 46;
        }
    }
    if (noterminal == "L")
    {
        if (terminalName == "Type")
        {
            return 30;
        }
        else
        {
            return 31;
        }
    }
    if (noterminal == "Param")
    {
        if (terminalName == "Type")
        {
            return 32;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "G")
    {
        if (terminalName == ",")
        {
            return 33;
        }
        else
        {
            return 34;
        }
    }
    if (noterminal == "Init")
    {
        if (terminalName == "Type")
        {
            return 35;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "H")
    {
        if (terminalName == "Identifier")
        {
            return 36;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Ident")
    {
        if (terminalName == "Identifier")
        {
            return 39;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "J")
    {
        if (terminalName == "Left index")
        {
            return 40;
        }
        else
        {
            return 41;
        }
    }
    if (noterminal == "K")
    {
        if (terminalName == "Identifier")
        {
            return 43;
        }
        else if (terminalName == "Constant")
        {
            return 42;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "M")
    {
        if (terminalName == "Identifier")
        {
            return 44;
        }
        else if (terminalName == "Constant")
        {
            return 44;
        }
        else if (terminalName == "Operator 2" && token == "-")
        {
            return 44;
        }
        else
        {
            return 45;
        }
    }
    if (noterminal == "N")
    {
        if (terminalName == "Operator =")
        {
            return 38;
        }
        else if (terminalName == "Semi")
        {
            return 37;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}