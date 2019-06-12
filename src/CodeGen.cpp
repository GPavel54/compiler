#include "CodeGen.h"

CodeGen::CodeGen(Lexer& lexer)
{
    tokens = lexer.tokens_;
}

void CodeGen::generateAsm()
{
    separateToFunctions();
    separateFunc(*(functions.begin()));
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
            hmap.insert(make_pair(tmp->token, FUNCTION_));
            types.push_back({tmp->token, (--tmp)->token, FUNCTION_, tmp->token});
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
    /*
    *   Блок обработки функции main
    */
    if (func.begin()->token == "{")
    {
        func.pop_front();
        func.pop_back();
        for (auto i = func.begin(); i != func.end();)
        {
            list<Token>::iterator todel = func.end();
            if (i->name == "Type")
            { //Значит идет объявление глобальной переменной
                string type = i->token;
                todel = i;
                i++;
                if (hmap.find(i->token) != hmap.end())
                {
                    throw Mdefinition_exception(i->token, i->row, i->col);
                }
                else
                {
                    hmap.insert(make_pair(i->token, 0));
                    types.push_back({i->token, type, 0, i->token});
                }
                auto ifindex = i;
                ifindex++;
                if (ifindex->token != "[")
                {
                    string out = (type == "char") ? " resb 1" : " resq 1";
                    bss << "\t" << i->token << out << endl;
                    if ((++i)->token == ";")
                    {
                        func.erase(todel, ++i);
                        i = func.begin();
                        continue;
                    }
                }
                else
                {
                    auto stringliteral = ifindex;
                    stringliteral++;
                    stringliteral++;
                    stringliteral++;
                    stringliteral++;
                    if (stringliteral->name == "String literal")
                    {
                        text << "\t" << i->token << " db " << stringliteral->token << ",0" << endl;
                        stringliteral++;
                        func.erase(todel, ++stringliteral);
                        i = func.begin();
                        continue;
                    }
                    else
                    {
                        string out = (type == "char") ? " resb " : " resq "; 
                        string size = (++ifindex)->token;
                        bss << "\t" << i->token << out << size << endl;
                        ifindex++;
                        if ((++ifindex)->token == ";")
                        {
                            func.erase(todel, ++ifindex);
                            i = func.begin();
                            continue;
                        }
                    }
                }
            }
            else if (i->name == "Identifier")
            {
                auto paranth = ++i;
                i--;
                cout << "Checking : " << i->token << endl;
                cout << "Second : " << paranth->token << endl;
                if (paranth->token == "(") // вызов функции
                {
                    if (i->token == "print")
                    {
                        if ((++i)->name == "String literal")
                        {
                            
                        }
                    }
                    cout << i->name << " --- function calling" << endl;
                    auto shift = i;
                    do
                    {
                        shift++;
                    } while (shift->token != ";");
                    shift++;
                    cout << "shift to " << shift->token << " row " << shift->row << endl;
                    func.erase(i, shift);
                    i = func.begin();
                }
                else if (paranth->name != "Logical operator") // присваивание
                { 
                    auto ind = i;
                    ind++;
                    if (ind->token != "[")
                    {
                        auto token = i;
                        auto exprStart = ++i;
                        i++;
                        exprStart++; // сдвинуть до первого токена после =
                        auto exprEnd = exprStart;
                        while (exprEnd->token != ";")
                        {
                            exprEnd++;
                        }
                        vector<Token> expression;
                        for (auto j = exprStart; j != exprEnd; j++)
                        {
                            expression.push_back(*j);
                        }
                        processExpr(*token, expression);
                        func.erase(token, ++exprEnd);
                        i = func.begin();
                        continue;
                    }
                    else
                    {
                        /* присваение в массив*/
                        cout << "Input to array" << endl;
                    }
                }
            }
            if (todel != func.end())
            {
                func.erase(todel);
                i = func.begin();
                continue;
            }
            i++;
        }
    }
    cout << "After type deleting : " << endl;
    for (auto i = func.begin(); i != func.end(); i++)
    {
        cout << i->token << " ";
    }
    cout << endl;
    asmfile << "section .bss" << endl;
    asmfile << bss.str();
    asmfile << "section .text" << endl;
    asmfile << "\t global _start" << endl;
    asmfile << "_start:" << endl;
    asmfile << text.str();
    asmfile << "\tmov rax, 60" << endl;
    asmfile << "\tmov rdi, 0" << endl;
    asmfile << "\tsyscall" << endl;
    cout << asmfile.str();
    // cout << "hmap:" << endl;
    // for (auto i = hmap.begin(); i != hmap.end(); i++)
    // {
    //     cout << i->first << " : " << i->second << endl;
    // }
    // cout << "Vector : " << endl;
    // for (auto i = types.begin(); i < types.end(); i++)
    // {
    //     cout << i->ident << " : " << i->level << " : " << i->type << endl;
    // }
}

void CodeGen::processExpr(Token left, vector<Token>& expression)
{
    translateToRpn(expression);
    cout << "expr" << endl;
    for (auto i = expression.begin(); i < expression.end(); i++)
    {
        cout << i->token << " ";
    }
    cout << endl;
    for (auto i = expression.begin(); i < expression.end(); i++)
    {
        if (i->name == "Constant")
        {
            text << "\t" << "push " << i->token << endl;
        }
        else if (i->name == "Operator 2")
        {
            text << "\t" << "pop rax" << endl;
            text << "\t" << "mov rcx, rax" << endl;
            text << "\t" << "pop rax" << endl;
            if (i->token == "+")
            {
                text << "\t" << "add rax,rcx" << endl;
            } 
            else
            {
                text << "\t" << "sub rax,rcx" << endl;
            }
            text << "\t" << "push rax" << endl;
        }
        else if (i->name == "Operator 1")
        {
            text << "\t" << "pop rax" << endl;
            text << "\t" << "mov rcx, rax" << endl;
            text << "\t" << "pop rax" << endl;
            if (i->token == "*")
            {
                text << "\t" << "mul rcx" << endl;
            } 
            else
            {
                text << "\t" << "div rcx" << endl;
            }
            text << "\t" << "push rax" << endl;
        }
        else if (i->name == "Identifier")
        {
            if ((i+1)->token == "[")
            {
                bool noexept = false;
                string address;
                for (auto j = types.end() - 1; j > types.begin(); j--)
                {
                    if (j->ident == i->token)
                    {
                        address = j->address;
                        noexept = true; 
                    }
                }
                if (!noexept)
                {
                    throw Ndefined_exception(i->token, i->row, i->col); 
                }
                if ((i+2)->name == "Identifier")
                {
                    string address;
                    getVarAddress(*(i+2), address);
                    text << "\t" << "mov rax, [" << address << "]" << endl;
                    getVarAddress(*i, address);
                    text << "\tpush qword [" << address << "+rax]" << endl;
                    i += 3;
                }
                else
                {
                    getVarAddress(*i, address);
                    text << "\t" << "push qword [" << address << "+" << (i+2)->token << "]" << endl;
                    i += 3;
                }
            }
            else
            {
                bool noexept = false;
                for (auto j = types.end() - 1; j > types.begin(); j--)
                {
                    if (j->ident == i->token)
                    {
                        text << "\t" << "push qword " << "[" << j->address << "]" << endl;
                        noexept = true; 
                    }
                }
                if (!noexept)
                {
                    throw Ndefined_exception(i->token, i->row, i->col); 
                }
            }
        }
    }
    bool noexept = false;
    for (auto i = types.end() - 1; i > types.begin(); i--)
    {
        if (i->ident == left.token)
        {
            text << "\tpop qword " << "[" << i->address << "]" << endl;
            noexept = true; 
        }
    }
    if (!noexept)
    {
        throw Ndefined_exception(left.token, left.row, left.col); 
    }
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

void CodeGen::getVarAddress(Token& var, string& address)
{
    bool noexept = false;
    for (auto j = types.end() - 1; j > types.begin(); j--)
    {
        if (j->ident == var.token)
        {
            address = j->address;
            noexept = true;
        }
    }
    if (!noexept)
    {
        throw Ndefined_exception(var.token, var.row, var.col); 
    }
}