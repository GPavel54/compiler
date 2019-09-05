#include "Parser.h"

Parser::Parser()
{
    ParsElem tmp;
    tmp.terminal = false;
    tmp.prod = "S";
    stack.push_back(tmp);
    products.push_back(&syntaxTree);
    ifstream in;
    in.open("table.txt", ifstream::in);
    if (!in.is_open())
    {
        throw (Open_exception("table.txt"));
    }
    string buffer;
    commands.push_back("");
    while (getline(in, buffer))
    {
        commands.push_back(buffer);
    }
    in.close();
}

void Parser::makeSyntaxTree(Lexer& lex)
{
    int ret = 0;
    while (1)
    { 
        if (!lex.tokens_.empty())
        {
            int firstRule = 1;
            if (lex.tokens_.front().name == "Identifier"){
                auto elem = lex.tokens_.begin();
                advance(elem, 1);
                if (elem->name == "Left parenthesis")
                {
                    firstRule = 0;
                }
            }
            ret = uncover(lex.tokens_.front(), firstRule);
            if (ret != 0 && ret != REMOVESYM)
            {
                // addToTree(ret);
                tree << commands[ret] << endl;
            }
            else if (ret == REMOVESYM)
            {
            //     bool needBreak = false;
            //     treeNode * tmpNode = products.front();
            //     while (needBreak == false)
            //     {
            //         for (int i = 0; i < tmpNode->size; i++)
            //         {
            //             if (tmpNode->nodes[i].name != "empty")
            //             {
            //                 continue;
            //             } 
            //             else 
            //             {
            //                 tmpNode->nodes[i].name = "term";
            //                 tmpNode->nodes[i].token = lex.tokens_.front().token;
            //                 needBreak = true;
            //                 break;
            //             }
            //         }
            //         if (needBreak == false)
            //         {
            //             products.erase(products.begin());
            //         }
            //     }
                lex.tokens_.pop_front();
            }
            else if (ret == 0)
            {
                break;
            }
        }   
        else
        {
            break;
        }
    }
    if (ret == 0)
    {
        /*
        *    Перевести это в функцию throw
        *         
        */
        throw Syntax_exception(lex.tokens_.front().token, lex.tokens_.front().row, lex.tokens_.front().col);
        // cout << "Error in your program in row = " << lex.tokens_.front().row << " col = " 
        //     << lex.tokens_.front().col << " name = " << lex.tokens_.front().token << endl;
    }
}

int Parser::uncover(Token& token, int rule)
{
    int ret = 0;
    if (!stack.back().terminal)
    {
        ret = addToStack(token.name, token.token, stack.back().prod, rule);
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

void Parser::addToTree(int rule)
{
    treeNode * prod = products.back(); 
    switch(rule){
        case 1:
            prod->terminal = false;
            prod->name = "S";
            prod->size = 13;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[11]);
            products.push_back(&((treeNode *)prod)->nodes[9]);
            break;
        case 2:
        case 3:
        case 4:
            prod->terminal = false;
            prod->name = "Program";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 5:
            prod->terminal = false;
            prod->name = "Program";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
        case 6:
            prod->terminal = false;
            prod->name = "Program"; // эпсилон
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 7:
            prod->terminal = false;
            prod->name = "Counting";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 8:
            prod->terminal = false;
            prod->name = "Counting";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            //products.push_back();
            break;
        case 9:
            prod->terminal = false;
            prod->name = "Expr";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 10:
            prod->terminal = false;
            prod->name = "Formula";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);          
            products.push_back(&((treeNode *)prod)->nodes[1]);            
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 11:
            prod->terminal = false;
            prod->name = "B";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[2]);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 12:
            prod->terminal = false;
            prod->name = "B"; // эпсилон
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 13:
            prod->terminal = false;
            prod->name = "A";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);            
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 14:
            prod->terminal = false;
            prod->name = "D";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[2]);            
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 15:
            prod->terminal = false;
            prod->name = "D"; // эпсилон
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 16:
            prod->terminal = false;
            prod->name = "C";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 17:
            prod->terminal = false;
            prod->name = "C";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 18:
            prod->terminal = false;
            prod->name = "Number";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 19:
            prod->terminal = false;
            prod->name = "Number";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
        case 20:
            prod->terminal = false;
            prod->name = "Number";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
        case 21:
            prod->terminal = false;
            prod->name = "Condition";
            prod->size = 8;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[5]);
            products.push_back(&((treeNode *)prod)->nodes[2]);
            break;
        case 22:
            prod->terminal = false;
            prod->name = "E";
            prod->size = 4;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[2]);
            break;
        case 23:
            prod->terminal = false;
            prod->name = "E"; // эпсилон
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 24:
            prod->terminal = false;
            prod->name = "Logical";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 25:
            prod->terminal = false;
            prod->name = "Logical";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[2]);
            break;
        case 26:
            prod->terminal = false;
            prod->name = "F";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 27:
            prod->terminal = false;
            prod->name = "F";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 28:
            prod->terminal = false;
            prod->name = "Loop";
            prod->size = 7;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[2]);
            products.push_back(&((treeNode *)prod)->nodes[5]);
            break;
        case 29:
            prod->terminal = false;
            prod->name = "Function";
            prod->size = 9;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[8]);
            products.push_back(&((treeNode *)prod)->nodes[6]);
            products.push_back(&((treeNode *)prod)->nodes[3]);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 30:
            prod->terminal = false;
            prod->name = "L";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 31:
            prod->terminal = false;
            prod->name = "L";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 32:
            prod->terminal = false;
            prod->name = "Param";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 33:
            prod->terminal = false;
            prod->name = "G";
            prod->size = 4;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[2]);
            break;
        case 34:
            prod->terminal = false;
            prod->name = "G";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 35:
            prod->terminal = false;
            prod->name = "Init";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 36:
            prod->terminal = false;
            prod->name = "H";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 37:
            prod->terminal = false;
            prod->name = "N";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
        case 38:
            prod->terminal = false;
            prod->name = "N";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 39:
            prod->terminal = false;
            prod->name = "Ident";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 40:
            prod->terminal = false;
            prod->name = "J";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 41:
            prod->terminal = false;
            prod->name = "J";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 42:
            prod->terminal = false;
            prod->name = "K";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
        case 43:
            prod->terminal = false;
            prod->name = "K";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 44:
            prod->terminal = false;
            prod->name = "M";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 45:
            prod->terminal = false;
            prod->name = "M";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 46:
            prod->terminal = false;
            prod->name = "Function";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 47:
            prod->terminal = false;
            prod->name = "Prog";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
        case 48:
            prod->terminal = false;
            prod->name = "Count";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 49:
            prod->terminal = false;
            prod->name = "O";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 50:
            prod->terminal = false;
            prod->name = "P";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 51:
            prod->terminal = false;
            prod->name = "P";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            prod->nodes[0].terminal = true;
            prod->nodes[0].token = "ε";
            break;
        case 52:
            prod->terminal = false;
            prod->name = "CountFunction";
            prod->size = 4;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[2]);
            break;
        case 53:
            prod->terminal = false;
            prod->name = "Number";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 54:
            prod->terminal = false;
            prod->name = "Prog";
            prod->size = 2;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            products.push_back(&((treeNode *)prod)->nodes[0]);
            break;
        case 55:
            prod->terminal = false;
            prod->name = "Block";
            prod->size = 3;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            products.push_back(&((treeNode *)prod)->nodes[1]);
            break;
        case 56:
            prod->terminal = false;
            prod->name = "Number";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
        case 57:
            prod->terminal = false;
            prod->name = "Number";
            prod->size = 1;
            prod->nodes = (treeNode *)malloc(sizeof(treeNode) * prod->size);
            break;
    }
    // prod->setEmpty();
}

void Parser::treeNode::setEmpty()
{
    for (int i = 0; i < size; i++)
    {
        if (nodes[i].name != "ε"){
            nodes[i].name = "empty";
        }
    }
}

void Parser::treeNode::printTree(treeNode& tn)
{
    vector<toPrintSt> toPrint;
    bool stop = false;
    treeNode * tmp = &tn;
    toPrintSt tempSt;
    tempSt.tn = tmp;
    tempSt.layer = 0;
    toPrint.push_back(tempSt);
    int index = 1, maxLayer = 0;
    while (!stop)
    {
        tmp = toPrint[index].tn;
        for (int i = 0; i < tmp->size; i++)
        {
            if (tmp->nodes[i].terminal == false)
            {
                tempSt.tn = tmp->nodes + i;
                tempSt.layer = index;
                toPrint.push_back(tempSt);
                if (index > maxLayer)
                {
                    maxLayer = index;
                }
            }
        }
        index++;
    }
    index = 0;
    stringstream ss;
    int currentLayer = 0;
    while (!toPrint.empty())
    {
        bool needPrintTab = true;
        for (auto i = toPrint.begin(); i < toPrint.end(); i++)
        {
            if (i->layer == currentLayer)
            {
                if (needPrintTab)
                {
                    for (int u = 0; u < maxLayer - currentLayer; u++)
                    {
                        ss << "\t";
                    }
                }
                for (int u = 0; u < i->tn->size; u++)
                {
                    if (i->tn->nodes[u].terminal)
                    {
                        ss << i->tn->nodes[u].token;
                    }
                    else
                    {
                        ss << i->tn->nodes[u].name;
                    }
                }
                toPrint.erase(i);
            }
        }
        currentLayer++;
        ss << "\n";
    }
    cout << "Syntax tree" << endl;
    cout << ss.str() << endl;
}

int Parser::addToStack(string terminalName, string token, string noterminal, int rule)
{
    int ret = getRule(terminalName, token, noterminal, rule);
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
            tmp.tok.name = "static";
            stack.push_back(tmp);
            tmp.tok.name = "Left brace";
            stack.push_back(tmp);
            tmp.tok.name = "Identifier";
            stack.push_back(tmp);
            tmp.tok.name = "Class";
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
            return 6;
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
            return 12;
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
            return 15;
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
            return 23;
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
            tmp.prod = "Param";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left parenthesis";
            stack.push_back(tmp);
            tmp.tok.name = "Identifier";
            stack.push_back(tmp);
            tmp.tok.name = "Type";
            stack.push_back(tmp);
            break;
        case 30:
            tmp.terminal = false;
            tmp.prod = "Function";
            stack.push_back(tmp);
            break;
        case 31:
            return 31;
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
            return 34;
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
            return 41;
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
            return 45;
        case 46:
            return 46;
        case 47:
            tmp.terminal = true;
            tmp.tok.name = "Semi";
            stack.push_back(tmp);
            tmp.tok.name = "break";
            stack.push_back(tmp);
            break;
        case 48:
            tmp.terminal = true;
            tmp.tok.name = "Semi";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "CountFunction";
            stack.push_back(tmp);
            break;
        case 49:
            tmp.terminal = false;
            tmp.prod = "P";
            stack.push_back(tmp);
            tmp.prod = "Number";
            stack.push_back(tmp);
            break;
        case 50:
            tmp.terminal = false;
            tmp.prod = "P";
            stack.push_back(tmp);
            tmp.prod = "Number";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = ",";
            stack.push_back(tmp);
            break;
        case 51:
            return 51;
        case 52:
            tmp.terminal = true;
            tmp.tok.name = "Right parenthesis";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "O";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left parenthesis";
            stack.push_back(tmp);
            tmp.tok.name = "Identifier";
            stack.push_back(tmp);
            break;
        case 53:
            tmp.terminal = false;
            tmp.prod = "CountFunction";
            stack.push_back(tmp);
            break;
        case 54:
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.prod = "Block";
            stack.push_back(tmp);
            break;
        case 55:
            tmp.terminal = true;
            tmp.tok.name = "Right brace";
            stack.push_back(tmp);
            tmp.terminal = false;
            tmp.prod = "Prog";
            stack.push_back(tmp);
            tmp.terminal = true;
            tmp.tok.name = "Left brace";
            stack.push_back(tmp);
            break;
        case 56:
            tmp.terminal = true;
            tmp.tok.name = "String literal";
            stack.push_back(tmp);
            break;
        case 57:
            tmp.terminal = true;
            tmp.tok.name = "Symbol";
            stack.push_back(tmp);
            break;
        case 58:
            tmp.terminal = false;
            tmp.prod = "Formula";
            stack.push_back(tmp);
            break;
    }
    return ret;
}

int Parser::getRule(string terminalName, string token, string noterminal, int rule)
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
        else if (terminalName == "break")
        {
            return 47;
        }
        else if (terminalName == "Left brace")
        {
            return 54;
        }
        else
        {
            return 6;
        }
    }
    if (noterminal == "Count")
    {
        if (terminalName == "Identifier" && rule == 1)
        {
            return 8;
        }
        else if (terminalName == "Identifier" && rule == 0)
        {
            return 48;
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
        else if (terminalName == "String literal")
        {
            return 10;
        }
        else if (terminalName == "Symbol")
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
        else if (terminalName == "String literal")
        {
            return 13;
        }
        else if (terminalName == "Symbol")
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
        else if (terminalName == "String literal")
        {
            return 17;
        }
        else if (terminalName == "Symbol")
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
        if (terminalName == "Identifier" && rule == 1)
        {
            return 18;
        }
        else if (terminalName == "Identifier" && rule == 0)
        {
            return 53;
        }
        else if (terminalName == "Constant")
        {
            return 19;
        }
        else if (terminalName == "Operator 2" && token == "-")
        {
            return 20;
        }
        else if (terminalName == "String literal")
        {
            return 56;
        }
        else if (terminalName == "Symbol")
        {
            return 57;
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
        if (terminalName == "Identifier" || terminalName == "Constant")
        {
            return 58;
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
        else if (terminalName == "String literal")
        {
            return 44;
        }
        else if (terminalName == "Symbol")
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
    if (noterminal == "O")
    {
        if (terminalName == "Identifier")
        {
            return 49;
        }
        else if (terminalName == "Constatnt")
        {
            return 49;
        }
        else if (terminalName == "Operator 2" && token == "-")
        {
            return 49;
        }
        else if (terminalName == "String literal")
        {
            return 49;
        }
        else if (terminalName == "Symbol")
        {
            return 49;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "P")
    {
        if (terminalName == ",")
        {
            return 50;
        }
        else
        {
            return 51;
        }
    }
    if (noterminal == "CountFunction")
    {
        if (terminalName == "Identifier")
        {
            return 52;
        }
        else
        {
            return 0;
        }
    }
    if (noterminal == "Block")
    {
        if (terminalName == "Left brace")
        {
            return 55;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

void Parser::printTree()
{
    cout << tree.str() << endl;
}