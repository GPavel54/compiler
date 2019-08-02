#include "CodeGen.h"

CodeGen::CodeGen(Lexer& lexer)
{
    tokens = lexer.tokens_;
}

void CodeGen::generateAsm()
{
    separateToFunctions();
    createSymbolicTable(*(functions.begin()));
    separateFunc(*(functions.begin()));

    cout << "hmap is :" << endl;
    int u = 1;
    for (auto i = hmap.begin(); i != hmap.end(); i++)
    {
        cout << u++ << ": " << i->first << " ; " << i->second.type << " ; " << i->second.level << " ; " << i->second.address << endl; 
    }
    cout << "ASM: " << endl;
    createAsm();
    cout << asmfile.str() << endl;
}

void CodeGen::separateToFunctions()
{
    list<Token>::iterator lbrace = tokens.end();
    list<Token>::iterator rbrace = tokens.end();
    /*
    * Выбор блока с главной функцией
    * 
    */
    tokens.pop_back();
    for (auto i = tokens.begin(); i != tokens.end(); i++)
    {
        if (i->token == "{")
        {
            tokens.erase(i);
            break;
        }
    }
    int lbraces = 0;
    for (auto i = tokens.begin(); i != tokens.end(); i++)
    {
        if (lbrace == tokens.end() && i->token == "{")
        {
            lbrace = i;
            continue;
        }
        if (i->token == "{")
        {
            lbraces++;
            continue;
        }
        if (i->token == "}")
        {
            if (lbraces > 0)
            {
                lbraces--;
                continue;
            }
            else if (lbraces == 0)
            {
                rbrace = i;
                break;
            }
        }
    }
    tokens.erase(tokens.begin(), lbrace);
    functions.push_back(list<Token>());
    functions.begin()->splice(functions.begin()->begin(), tokens, lbrace, ++rbrace);
    
    /*
    * Занесение в список всех пользовательских функций
    * 
    */
    while (!tokens.empty())
    {
        lbrace = tokens.end();
        rbrace = tokens.end();
        lbraces = 0;
        for (auto i = tokens.begin(); i != tokens.end(); i++)
        {
            if (i->token == "{" && lbrace == tokens.end())
            {
                lbrace = i;
                continue;
            }
            if (i->token == "{")
            {
                lbraces++;
                continue;
            }
            if (i->token == "}")
            {
                if (lbraces > 0)
                {
                    lbraces--;
                    continue;
                }
                else if (lbraces == 0)
                {
                    rbrace = i;
                    break;
                }
            } 
        }
        functions.push_back(list<Token>(tokens.begin(), rbrace));
        tokens.erase(tokens.begin(), ++rbrace);
    }
    auto point = ++functions.begin();
    while (point != functions.end())
    {
        auto tmp = point->begin();
        tmp++;
        if (hmap.find(tmp->token) != hmap.end())
        {
            throw Mdefinition_exception(tmp->token, tmp->row, tmp->col);
        }
        else
        {
            Info toTable = {(--tmp)->token, FUNCTION_, tmp->token};
            hmap.insert(make_pair(tmp->token, toTable));
        }
        point++;
    }
    /*
    *   В этой точке записаны все идентификаторы пользовательских функций
    * 
    */
}

void CodeGen::separateFunc(list<Token>& func)
{
    int level = 0, size = 1;
    int bp = 0;
    for (auto i = func.begin(); i != func.end(); i++)
    {
        if (i->name == "Type") // Обработка строки, которая начинается с названия типа
        {
            bool cmp = false;
            Token type = *i;
            if (strcmp(i->token.c_str(), "char") == 0)
            {
                cmp = true;
            }
            Token name = *(++i);
            bool itsarray = false;
            if ((++i)->name == "Left index") // Объявление массива
            {
                itsarray = true;
                Token arrSize = *(++i);
                if (arrSize.name != "Constant" || (++i)->name != "Right index")
                {
                    throw StaticSize_exception(arrSize.token, arrSize.row, arrSize.col);
                }
                size = stoi(arrSize.token);
            }
            else
            {
                i--; // вернуться к имени
            }
            addVariable((cmp ? 2 : 4) * size, name, type.token, level, bp);
            if ((++i)->name == "Semi")
            {
                continue;
            }
            else if (i->name == "Operator =")
            {
                if (type.token == "int" && itsarray) // массив не инициализируется
                {
                    throw ArrayInit_exception(i->token, i->row, i->col);
                }
                else if (type.token == "int")
                {
                    vector<Token> expr;
                    while ((++i)->name != "Semi")
                    {
                        expr.push_back(*i);
                    }
                    cout << "Expr before Translate" << endl;
                    printExpr(expr);
                    translateToRpn(expr);
                    cout << "Expr after Translate" << endl;
                    printExpr(expr);
                }
            }
        }
    }
    cout << "---end---" << endl;
}

void CodeGen::processExpr(Token left, vector<Token>& expression)
{
    
}

void CodeGen::translateToRpn(vector<Token>& expression)
{
    // cout << "Выражение" << endl;
    // for (auto i = expression.begin(); i < expression.end();i++)
    // {
    //     cout << i->token << " ";
    // }
    cout << endl;
    vector<Token> stack;
    vector<Token> out;
    for (auto i = expression.begin(); i < expression.end(); i++)
    {
        if (i->name == "Constant")
        {
            out.push_back(*i);
            continue;
        }
        else if (i->name == "Identifier")
        {
            if ((i+1)->token == "[")
            {
                while (i->token != "]")
                {
                    out.push_back(*i);
                    i++;
                }
                out.push_back(*i);
                continue;
            }
            else 
            {
                out.push_back(*i);
                continue;
            }
        }
        else if (i->name == "Operator 1")
        {
            if (stack.empty())
            {
                stack.push_back(*i);
                continue;
            }
            auto j = --(stack.end());
            while (j->name == "Operator 1")
            {
                out.push_back(*j);
                j--;
                stack.pop_back();
                if (stack.empty())
                {
                    break;
                }
            }
            stack.push_back(*i);
        }
        else if (i->name == "Left parenthesis")
        {
            stack.push_back(*i);
        }
        else if (i->name == "Right parenthesis")
        {
            auto j = --(stack.end());
            while (j->name != "Left parenthesis")
            {
                out.push_back(*j);
                j--;
                stack.pop_back();
                if (stack.empty())
                {
                    break;
                }
            }
            stack.pop_back();
            continue;
        }
        else if (i->name == "Operator 2")
        {
            if (stack.empty())
            {
                stack.push_back(*i);
                continue;
            }
            auto j = --(stack.end());
            while (j->name == "Operator 2" || j->name == "Operator 1")
            {
                out.push_back(*j);
                j--;
                stack.pop_back();
                if (stack.empty())
                {
                    break;
                }
            }
            stack.push_back(*i);
        }
    }
    while (!stack.empty())
    {
        auto j = stack.end() - 1;
        out.push_back(*j);
        j--;
        stack.pop_back();
    }
    // cout << "expr" << endl;
    // for (auto i = out.begin(); i < out.end(); i++)
    // {
    //     cout << i->token << " ";
    // }
    // cout << endl;
    expression = out;
}

void CodeGen::createSymbolicTable(list<Token>& func)
{
    vector<string> variables;
    for (auto i = func.begin(); i != func.end(); i++)
    {
        if (i->name == "Type")
        {
            variables.push_back((++i)->token);
        }
    }
    symbolicTable.push_back(variables);
}

void CodeGen::addVariable(int size, Token& token, string& type, int level, int& bp)
{
    static int sp = 0;
    auto check = hmap.find(token.token); 
    if (check != hmap.end() && check->second.level == level)
    {
        throw Mdefinition_exception(token.token, token.row, token.col);
    }
    sp += size;
    stringstream ss;
    ss << sp;
    Info tmp = {type, level, ss.str()};
    hmap.insert(make_pair(token.token, tmp));
    text << "sub rsp, " << size << " ; allocate memory for var " << token.token << endl; // выделение памяти путем смещения вершины стека
}

void CodeGen::createAsm()
{
    asmfile << "section .text" << endl;
    asmfile << "\tglobal _start" << endl;
    asmfile << "_start:" << endl;
    asmfile << "mov rbp, rsp" << endl;
    asmfile << text.str() << endl;
}


void CodeGen::printExpr(vector<Token>& expr)
{
    for (auto i = expr.begin(); i != expr.end(); i++)
    {
        cout << i->token << " ";
    }
    cout << endl;
}
