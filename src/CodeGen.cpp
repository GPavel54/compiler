#include "CodeGen.h"

CodeGen::CodeGen(Lexer& lexer)
{
    tokens = lexer.tokens_;
    sp = 0;
}

void CodeGen::generateAsm()
{
    separateToFunctions();
    createSymbolicTable(*(functions.begin()));
    separateFunc(*(functions.begin()));

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
            Info toTable = {(--tmp)->token, FUNCTION_, tmp->token, 1};
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
    int level = 0;
    int bp = 0;
    for (auto i = func.begin(); i != func.end(); i++)
    {
        int size = 1;
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
                i--; // вернуться в списке токенов к имени
            }
            addVariable((cmp ? 2 : 4) * size, name, type.token, level, bp);
            if ((++i)->name == "Semi")
            {
                continue;
            }
            else if (i->name == "Operator =")  // инициальзация переменных
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
                    translateToRpn(expr);
                    cout << "Expr after Translate" << endl;
                    printExpr(expr);
                    text << "; translated initiation exrpession" << endl;
                    processExpr(name, expr);
                }
            }
        }
        else if (i->name == "Left brace") // открытие блока
        {
            level++;
        }
        else if (i->name == "Right brace") // закрытие блока 
        {
            int addToSP = 0;
            for (auto j = hmap.begin(); j != hmap.end();)
            {
                if (j->second.level == level)
                {
                    addToSP += j->second.size;
                    hmap.erase(j);
                    j = hmap.begin();
                    continue;
                }
                j++;
            }
            if (addToSP != 0)
            {
                text << "add rsp, " << addToSP << "; free memory after left code block" << endl;
            }
            level--;
        }
    }
}

/*
 * В эту функцию передается выражение,  
 * переведенное в обратную польскую запись
*/
void CodeGen::processExpr(Token left, vector<Token>& expression)
{
    cout << "name = " << left.token << endl;
    printExpr(expression);
    for (auto i = expression.begin(); i != expression.end(); i++)
    {
        if (i->name == "Identifier")
        {
            Token name = *i;
            if (hmap.find(i->token) == hmap.end())
            {
                throw Ndefined_exception(i->token, i->row, i->col);
            }
            if ((++i)->name == "Left index")
            {
                vector<Token>::iterator start = i;
                do
                {
                    i++;
                } while (i->name != "Right index");
                vector<Token> expr(start, i + 1);
                getArrayValue(name.token, expr);
            }
            else
            {
                // text << "push " << (hmap.find(i->token)->second.size == 2) ? "word " : "dword "; // это исправить
                //text << 
                // продолжить отсюда
            }
        }
        if (i->token == "Constant")
        {
            // text << "push " << i->token;
            text << "mov [rsp], dword " << i->token << endl;
            text << "sub rsp, 4" << endl;
        }
    }
}


/**
 * Обработка случая, если в индекс массива вложен другой иднекс
 * после ее исполнения на вершине стека окажется 4х байтное необходимое значение
*/
void CodeGen::getArrayValue(string& name, vector<Token> expression)
{
    cout << "trying to get array value of " << endl;
    printExpr(expression);
    for (auto i = expression.begin(); i != expression.end(); i++)
    {
        if (i->name == "Constant")
        {
            // text << "push dword " << i->token << endl;
            text << "mov [rsp], dword " << i->token << endl;
            text << "sub rsp, 4" << endl;
        }
        else if (i->name == "Identifier")  // обработка идентификатора выражения
        {
            if (hmap.find(i->token) == hmap.end())
            {
                throw Ndefined_exception(i->token, i->row, i->col);
            }
            if ((i+1)->token == "[")
            {
                throw Nesting_exception((i+1)->token, (i+1)->row, (i+1)->col);
            }
            else
            {
                if (hmap.find(i->name)->second.type == "char")
                {
                    text << "xor eax, eax" << endl;
                    // text << "mov ax, [rbp - " << hmap.find(i->name)->second.address << "]" << endl;
                    text << "mov r8, rbp" << endl;
                    text << "sub r8, " << hmap.find(i->name)->second.address << endl;
                    text << "mov ax, [r8]" << endl;
                    text << "mov [rsp], eax" << endl;
                    text << "add rsp, 2" << endl;
                }
                else
                {
                    // text << "push dword [rbp - " << hmap.find(i->name)->second.address << "]" << endl;
                    text << "mov r8, rbp" << endl;
                    text << "sub r8, " << hmap.find(i->name)->second.address << endl;
                    text << "mov eax, [r8]" << endl;
                    text << "mov [rsp], eax" << endl;
                    text << "add rsp, 4" << endl;
                }
            }
        }
        else if (i->name == "Operator 1" || i->name == "Operator 2")
        {
            if (i->token == "+")
            {
                // text << "pop dword r8d" << endl;
                // text << "pop dword r9d" << endl;
                // text << "push dword r8d" << endl;
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov r9d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "add r8d, r9d" << endl;
                text << "mov [rsp], r8d" << endl;
                text << "add rsp, 4" << endl;
            }
            else if (i->token == "-")
            {
                // text << "pop dword r8d" << endl;
                // text << "pop dword r9d" << endl;
                // text << "push dword r9d" << endl;
                
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov r9d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "sub r9d, r8d" << endl;
                text << "mov [rsp], r9d" << endl;
                text << "add rsp, 4" << endl;
            }
            else if (i->token == "*")
            {
                // text << "pop dword r8d" << endl;
                // text << "pop dword r9d" << endl;
                // text << "mul r8d, r9d" << endl;
                // text << "push dword r8d" << endl;

                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov r9d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mul r8d, r9d" << endl;
                text << "mov [rsp], r8d" << endl;
                text << "add rsp, 4" << endl;
            }
            else if (i->token == "/")
            {
                // text << "pop dword eax" << endl;
                // text << "pop dword r8d" << endl;
                // text << "div r8d" << endl;
                // text << "push dword eax" << endl;

                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov eax, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "div r8d" << endl;
                text << "mov [rsp], eax" << endl;
                text << "add rsp, 4" << endl;

            }
        }
    }
    /**
     * Смещение от начала массива вычисленно и хранится на вершине стека
     * теперь необходимо вместо этого смещения поместить нужное значение
    */
    text << "mov r8d, [rsp]" << endl;
    text << "add rsp, 4" << endl;
    text << "mov r9, rbp" << endl;
    text << "sub r9, " << hmap.find(name)->second.address << endl;
    text << "add r9d, r8d" << endl;
}

void CodeGen::translateToRpn(vector<Token>& expression)
{
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
            if ((i + 1) != expression.end() && (i+1)->token == "[")
            {
                /*
                * Необходимо выполнить преобразование выражения, 
                * которое является смещением от начала вектора
                */
                out.push_back(*i); // имя массива
                out.push_back(*(++i));
                vector<Token>::iterator start = i + 1;
                int toSkip = 0;
                while (1)//i->token != "]"
                {
                    if (i->token == "[")
                    {
                        toSkip++;
                    }
                    if (i->token == "]")
                    {
                        toSkip--;
                        if (toSkip == 0)
                        {
                            break;
                        }
                    }
                    else
                    {
                    }
                    i++;
                }
                vector<Token>::iterator end = i;
                vector<Token> indexExpr(start, end);
                printExpr(indexExpr);
                translateToRpn(indexExpr);
                out.insert(out.end(), indexExpr.begin(), indexExpr.end());
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
    auto check = hmap.find(token.token); 
    if (check != hmap.end() && check->second.level == level)
    {
        throw Mdefinition_exception(token.token, token.row, token.col);
    }
    sp += size;
    stringstream ss;
    ss << sp;
    Info tmp = {type, level, ss.str(), size};
    hmap.insert(make_pair(token.token, tmp));
    text << "sub rsp, " << size << " ; allocate memory for var " << token.token << " level = " << level << endl; // выделение памяти путем смещения вершины стека
}

void CodeGen::createAsm()
{
    asmfile << "extern printf" << endl;
    asmfile << "section .data" << endl;
    asmfile << "formatint db '%d', 0" << endl;
    asmfile << "formatstr db '%s', 0" << endl;
    asmfile << "section .text" << endl;
    asmfile << "\tglobal main" << endl;
    asmfile << "main:" << endl;
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

void CodeGen::printHashTable()
{
    int counter = 1;
    for (auto j = hmap.begin(); j != hmap.end(); j++)
    {
        cout << counter << ": " << j->first << " | " << j->second.size << " | " << j->second.type << endl;
    }
}
