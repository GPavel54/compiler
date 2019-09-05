#include "CodeGen.h"

CodeGen::CodeGen(Lexer &lexer)
{
    tokens = lexer.tokens_;
    sp = 0;
    reserved_memory = 0;
    memory_counter = 0;
}

void CodeGen::generateAsm(string path)
{
    separateToFunctions();
    createSymbolicTable(*(functions.begin()));
    separateFunc(*(functions.begin()));

    createAsm();
    ofstream outfile;
    outfile.open(path,fstream::out);
    if (!outfile.is_open())
    {
        throw (Open_exception(path));
    } 
    outfile << asmfile.str() << endl;
    outfile.close();
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

void CodeGen::separateFunc(list<Token> &func)
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
            else if (i->name == "Operator =") // инициальзация переменных
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
                    processExpr(name, expr);
                    text << "; translated initiation exrpession" << endl;
                }
                else if (type.token == "char" && itsarray)
                {
                    while (i->name != "String literal")
                    {
                        i++;
                    }
                    if (i->token.size() > size - 1)
                    {
                        throw Size_expression(i->token, i->row, i->col);
                    }
                    int shift = 0;
                    auto j = i->token.begin();
                    for (++j; *j != '"'; j++)
                    {
                        if (shift == 0)
                        {
                            text << "mov [rsp], byte '" << *j << "'" << endl;
                            text << "mov [rsp + 1], byte 5" << endl;
                            shift = 2;
                        }
                        else
                        {
                            text << "mov [rsp + " << shift++ << "], byte '" << *j << "'" << endl;
                            text << "mov [rsp + " << shift++ << "], byte 5" << endl;
                        }
                    }
                    text << "mov [rsp + " << shift++ << "], byte 0" << endl;
                    text << "mov [rsp + " << shift++ << "], byte 0" << endl;
                }
                else if (type.token == "char")
                {
                    vector<Token> expr;
                    while ((++i)->name != "Semi")
                    {
                        expr.push_back(*i);
                    }
                    translateToRpn(expr);
                    processExpr(name, expr);
                    text << "; translated initiation exrpession" << endl;
                }
            }
        }
        else if (i->name == "Left brace") // открытие блока
        {
            level++;
            blocksType.push_back(nextBlock);
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
                memory_counter -= addToSP;
            }
            level--;
            nextBlock = blocksType.back();
            blocksType.pop_back();
            i++;
            if (i == func.end() || i->name != "else")
            {
                i--;
            }
            if (nextBlock.type == "if" && i->name != "else")
            {
                text << nextBlock.type << nextBlock.num << ":" << endl;
                nextBlock.type = "";
                nextBlock.num = 0;
            }
            else if (nextBlock.type == "if" && i->name == "else")
            {
                static int outif = 0;
                outif++;
                text << "jmp outelse" << outif << endl;
                text << nextBlock.type << nextBlock.num << ":" << endl;
                nextBlock.type = "outelse";
                nextBlock.num = outif;
                continue;
            }
            else if (nextBlock.type == "outelse")
            {
                text << nextBlock.type << nextBlock.num << ":" << endl;
                nextBlock.type = "";
                nextBlock.num = 0;
            }
        }
        if (i->name == "Identifier")
        {
            auto ident = i;
            if (i->token == "print") // Функция печати
            {
                auto err = i;
                i++;
                auto toPrint = ++i, begin = i; // begin - after (
                // Проверка на переданные параметры
                int args = 0;
                do
                {
                    i++;
                    if (i->name == "String literal")
                    {
                        args++;
                    }
                    else if (i->name == "Left index")
                    {
                        args++;
                        while (i->name != "Right index")
                        {
                            i++;
                        }
                    }
                } while (i->name != "Semi");
                if (args > 1)
                {
                    throw Print_exception(err->token, err->row, err->col);
                }
                i = begin;
                // Конец проверки
                if (hmap.find(toPrint->token) == hmap.end())
                {
                    throw Ndefined_exception(toPrint->token, toPrint->row, toPrint->col);
                }
                /**
                 *  Запись в eax значения, которое необходимо распечатать
                 */
                if ((++i)->name == "Right parenthesis" && (++i)->name == "Semi")
                {
                    text << "mov r8, rbp" << endl;
                    text << "sub r8, " << hmap.find(toPrint->token)->second.address << endl;
                    text << "xor eax, eax" << endl;
                    if (hmap.find(toPrint->token)->second.type == "char" && hmap.find(toPrint->token)->second.size == 2)
                    {
                        text << "mov ax, [r8]" << endl;
                        printVariable(CHAR);
                    }
                    else if (hmap.find(toPrint->token)->second.type == "char" && hmap.find(toPrint->token)->second.size != 2)
                    {
                        text << "mov rax, r8" << endl;
                        printVariable(STRING, hmap.find(toPrint->token)->second.address);
                    }
                    else
                    {
                        text << "mov eax, [r8]" << endl;
                        printVariable(INT);
                    }
                    //В eax находится значение, которое необходимо распечатать
                }
                else if (i->name == "Left index") // Распечатать значение массива
                {
                    vector<Token> expres;
                    while (i->name != "Right index")
                    {
                        expres.push_back(*i);
                        i++;
                    }
                    translateToRpn(expres);
                    processExpr(*toPrint, expres, -1);
                    text << "mov r8, rbp" << endl;
                    text << "sub r8, " << hmap.find(toPrint->token)->second.address << endl;
                    text << "mov r9, r8" << endl;
                    text << "shr r9, 32" << endl;
                    text << "shl r9, 32" << endl;
                    text << "add r8d, r12d" << endl;
                    text << "or r8, r9" << endl;
                    text << "mov eax, [r8]" << endl;

                    if (hmap.find(toPrint->token)->second.type == "char")
                    {
                        printVariable(CHAR);
                    }
                    else
                    {
                        printVariable(INT);
                    }
                }
                else if (i->name == "String literal") // Рапечатать строку
                {
                }
            }
            else if ((++i)->name == "Operator =") // Присвоение значения переменной
            {
                vector<Token> expr;
                while ((++i)->name != "Semi")
                {
                    expr.push_back(*i);
                }
                translateToRpn(expr);
                processExpr(*ident, expr);
                text << "; translated exrpession" << endl;
            }
            else if (i->name == "Left index") // Присваивание значения элементу массива
            {
                vector<Token> arrExpr;
                while (i->name != "Right index")
                {
                    i++;
                    arrExpr.push_back(*i);
                }
                arrExpr.pop_back();
                translateToRpn(arrExpr);
                processExpr(*ident, arrExpr, -1); // В регистре r12d теперь находится смещение от первого элемента массива
                arrExpr.clear();
                while ((++i)->name != "Semi")
                {
                    arrExpr.push_back(*i);
                }
                printExpr(arrExpr);
                translateToRpn(arrExpr);
                processExpr(*ident, arrExpr, -2);
            }
        }
        if (i->name == "if") // обработка условия
        {
            //Обработка значения слева от знака
            static int num = 0;
            i++;
            auto first = ++i;
            text << ";Compare two values" << endl
                 << "mov rax, rbp" << endl;
            if (first->name == "Constant")
            {
                text << "mov r8d, " << i->token << endl;
            }
            else if (first->name == "Identifier" && (++i)->name != "Left index")
            {
                text << "sub rax, " << hmap.find(first->token)->second.address << endl;
                if (hmap.find(first->token)->second.type == "int")
                {
                    text << "mov r8d, [rax]" << endl;
                }
                else
                {
                    text << "xor r8, r8" << endl;
                    text << "mov r8b, [rax]" << endl;
                }
            }
            else
            {
                vector<Token> expr;
                while ((i++)->name != "Right index")
                {
                    expr.push_back(*i);
                }
                // getArrayValue();
                // Доделать обработку переданного элемента массива
            }
            auto sign = i++;
            auto second = i;
            if (second->name == "Constant")
            {
                text << "mov r9d, " << second->token << endl;
            }
            else if (second->name == "Identifier" && (++i)->name != "Left index")
            {
                text << "sub rax, " << hmap.find(second->token)->second.address << endl;
                if (hmap.find(second->token)->second.type == "int")
                {
                    text << "mov r9d, [rax]" << endl;
                }
                else
                {
                    text << "xor r9, r9" << endl;
                    text << "mov r9b, [rax]" << endl;
                }
            }
            else
            {
                vector<Token> expr;
                while ((i++)->name != "Right index")
                {
                    expr.push_back(*i);
                }
            }
            text << "cmp r8d, r9d" << endl;
            if (sign->token == "==")
            {
                text << "jne if" << num << endl;
            }
            else if (sign->token == ">")
            {
                text << "jle if" << num << endl;
            }
            else if (sign->token == "<")
            {
                text << "jge if" << num << endl;
            }
            else if (sign->token == ">=")
            {
                text << "jl if" << num << endl;
            }
            else if (sign->token == "<=")
            {
                text << "jg if" << num << endl;
            }
            else // !=
            {
                text << "je if" << num << endl;
            }
            nextBlock.type = "if";
            nextBlock.num = num++;
        }
        if (i->name == "else")
        {
            
        }
    }
}

/*
 * В эту функцию передается выражение,  
 * переведенное в обратную польскую запись
*/
void CodeGen::processExpr(Token left, vector<Token> &expression, int shift)
{
    for (auto i = expression.begin(); i != expression.end(); i++)
    {
        if (i->name == "Identifier")
        {
            Token name = *i;
            if (hmap.find(i->token) == hmap.end())
            {
                throw Ndefined_exception(i->token, i->row, i->col);
            }
            if ((++i)->name == "Left index") // если эта переменная - массив
            {
                vector<Token>::iterator start = i;
                do
                {
                    i++;
                } while (i->name != "Right index");
                vector<Token> expr(start, i + 1);
                getArrayValue(name.token, expr);
            }
            else // просто переменная
            {
                i--;
                text << "mov r8, rbp" << endl;
                if (hmap.find(i->token)->second.type == "char")
                {
                    text << "xor eax, eax" << endl;
                    text << "sub r8, " << hmap.find(i->token)->second.address << endl;
                    text << "sub rsp, 4" << endl;
                    text << "mov ax, [r8]" << endl;
                    text << "mov [rsp], eax" << endl;
                }
                else
                {
                    text << "xor eax, eax" << endl;
                    text << "sub r8, " << hmap.find(i->token)->second.address << endl;
                    text << "sub rsp, 4" << endl;
                    text << "mov eax, [r8]" << endl;
                    text << "mov [rsp], eax" << endl;
                }
            }
        }
        else if (i->name == "Constant")
        {
            // text << "push " << i->token;
            text << "sub rsp, 4" << endl;
            text << "mov [rsp], dword " << i->token << endl;
        }
        else if (i->name == "Symbol")
        {
            // text << "push " << i->token;
            text << "sub rsp, 4" << endl;
            text << "mov [rsp], dword " << i->token << endl;
        }
        else if (i->name == "Operator 1" || i->name == "Operator 2")
        {
            if (i->token == "+")
            {
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov r9d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "add r8d, r9d" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], r8d" << endl;
            }
            else if (i->token == "-")
            {
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov r9d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "sub r9d, r8d" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], r9d" << endl;
            }
            else if (i->token == "*")
            {
                text << "mov eax, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov ebx, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "imul ebx" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], eax" << endl;
            }
            else if (i->token == "/")
            {
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov eax, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "div r8d" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], eax" << endl;
            }
        }
    }
    /**
     * На вершине стека находится значение вычисленного выражения.
     *  Необходимо записать его в заданную переменную
    */
    //Перенесли значение выражение в регистр
    text << "mov r8d, [rsp]" << endl;
    text << "add rsp, 4" << endl;

    if (shift == -1) //Значит необходимо вычислить смещение от начала массива
    {
        text << "mov r12d, r8d" << endl;
        text << "mov eax, r12d" << endl;
        if (hmap.find(left.token)->second.type == "char")
        {
            text << "mov ebx, 2" << endl;
        }
        else
        {
            text << "mov ebx, 4" << endl;
        }
        text << "imul ebx" << endl;
        text << "mov r12d, eax; shift from the start of array" << endl; // В r12d находится смещение от начала массива
        return;
    }
    if (shift == -2)
    {
        text << "mov r9, rbp" << endl;
        text << "sub r9, " << hmap.find(left.token)->second.address << endl;
        text << "mov r10, r9" << endl;
        text << "shr r10, 32" << endl;
        text << "shl r10, 32" << endl;
        text << "add r9d, r12d" << endl;
        text << "or r9, r10" << endl;

        if (hmap.find(left.token)->second.type == "char")
        {
            text << "or r8w, word 1280; set \005" << endl;
            text << "mov [r9], r8w; value to put to array" << endl;
        }
        else
        {
            text << "mov [r9], r8d; value to put to array" << endl;
        }
        return;
    }

    //Сместились к этой переменной
    text << "mov r9, rbp" << endl;
    text << "sub r9, " << hmap.find(left.token)->second.address << endl;

    if (shift != 0)
    {
        text << "add r9, " << shift << endl;
    }
    if (hmap.find(left.token)->second.type == "char")
    {
        text << "mov [r9], r8w" << endl;
    }
    else
    {
        text << "mov [r9], r8d" << endl;
    }
}

/**
 * Обработка случая, если в индекс массива вложен другой индекс
 * после ее исполнения на вершине стека окажется 4х байтное необходимое значение
*/
void CodeGen::getArrayValue(string &name, vector<Token> expression)
{
    printExpr(expression);
    for (auto i = expression.begin(); i != expression.end(); i++)
    {
        if (i->name == "Constant")
        {
            // text << "push dword " << i->token << endl;
            text << "sub rsp, 4" << endl;
            text << "mov [rsp], dword " << i->token << endl;
        }
        else if (i->name == "Identifier") // обработка идентификатора выражения
        {
            if (hmap.find(i->token) == hmap.end())
            {
                throw Ndefined_exception(i->token, i->row, i->col);
            }
            if ((i + 1)->token == "[")
            {
                throw Nesting_exception((i + 1)->token, (i + 1)->row, (i + 1)->col);
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
                    text << "sub rsp, 4" << endl;
                    text << "mov [rsp], eax" << endl;
                }
                else
                {
                    // text << "push dword [rbp - " << hmap.find(i->name)->second.address << "]" << endl;
                    text << "mov r8, rbp" << endl;
                    text << "sub r8, " << hmap.find(i->name)->second.address << endl;
                    text << "mov eax, [r8]" << endl;
                    text << "sub rsp, 4" << endl;
                    text << "mov [rsp], eax" << endl;
                }
            }
        }
        else if (i->name == "Operator 1" || i->name == "Operator 2")
        {
            if (i->token == "+")
            {
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov r9d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "add r8d, r9d" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], r8d" << endl;
            }
            else if (i->token == "-")
            {
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov r9d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "sub r9d, r8d" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], r9d" << endl;
            }
            else if (i->token == "*")
            {
                text << "mov eax, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov ebx, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "imul ebx" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], eax" << endl;
            }
            else if (i->token == "/")
            {
                text << "mov r8d, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "mov eax, [rsp]" << endl;
                text << "add rsp, 4" << endl;
                text << "div r8d" << endl;
                text << "sub rsp, 4" << endl;
                text << "mov [rsp], eax" << endl;
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
    text << "mov r10, r9" << endl;
    text << "add r9d, r8d" << endl;
    text << "shr r10, 32" << endl;
    text << "shl r10, 32" << endl;
    text << "or r9, r10" << endl;
    text << "mov r10d, [r9]" << endl;
    text << "sub rsp, 4 \t; mov to stack array value" << endl;
    text << "mov [rsp], r10d" << endl;
}

void CodeGen::translateToRpn(vector<Token> &expression)
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
        if (i->name == "Symbol")
        {
            out.push_back(*i);
            continue;
        }
        if (i->name == "String literal")
        {
            throw Wrong_expression(i->token, i->row, i->col);
        }
        else if (i->name == "Identifier")
        {
            if ((i + 1) != expression.end() && (i + 1)->token == "[")
            {
                /*
                * Необходимо выполнить преобразование выражения, 
                * которое является смещением от начала вектора
                */
                out.push_back(*i); // имя массива
                out.push_back(*(++i));
                vector<Token>::iterator start = i + 1;
                int toSkip = 0;
                while (1) //i->token != "]"
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

void CodeGen::createSymbolicTable(list<Token> &func)
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

void CodeGen::addVariable(int size, Token &token, string &type, int level, int &bp)
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
    memory_counter += size;
    if (memory_counter > reserved_memory)
    {
        reserved_memory = memory_counter;
    }
}

void CodeGen::createAsm()
{
    asmfile << "extern printf" << endl;
    asmfile << "section .data" << endl;
    asmfile << "formatint db '%d ', 0" << endl;
    asmfile << "formatstr db '%s', 0" << endl;
    asmfile << "formatchar db '%c', 0" << endl;
    asmfile << "section .bss" << endl;
    asmfile << "saved_stack: \t resw " << (reserved_memory) + 16 << "; this is the memory to copy stack to" << endl;
    asmfile << "_last_value: \t resq 1" << endl;
    asmfile << "section .text" << endl;
    asmfile << "\tglobal main" << endl;
    asmfile << "main:" << endl;
    asmfile << "mov rbp, rsp" << endl;
    asmfile << text.str() << endl;
    asmfile << "ret" << endl;
}

void CodeGen::printExpr(vector<Token> &expr)
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

void CodeGen::printVariable(CodeGen::TYPE type, string address)
{
    //Копирование стека в зарезервированную в bss область
    text << "mov [saved_stack], rbp" << endl;
    text << "mov [saved_stack + 8], rsp" << endl;
    text << "mov r9, rbp" << endl;
    text << "sub r9, rsp" << endl;
    text << "mov r11, saved_stack" << endl;
    text << "add r11, 16" << endl;

    static int o = 0;
    text << "move" << o << ":"
         << "cmp r9, 0" << endl;
    text << "je out" << o << endl;
    text << "mov r10w, [rsp]" << endl;
    text << "mov [r11], r10w" << endl;
    text << "add rsp, 2" << endl;
    text << "sub r9, 2" << endl;
    text << "add r11, 2" << endl;
    text << "jmp move" << o << endl;
    text << "out" << o << ":" << endl;
    text << "mov [_last_value], r11" << endl;
    //Стек перенесен в зарезервированную область
    if (type == CHAR)
    {
        text << "mov rdi, formatchar" << endl;
        text << "mov esi, eax" << endl;
        text << "xor rax, rax" << endl;
        text << "call printf" << endl;
    }
    else if (type == INT)
    {
        text << "mov rdi, formatint" << endl;
        text << "mov esi, eax" << endl;
        text << "xor rax, rax" << endl;
        text << "call printf" << endl;
    }
    else if (type == STRING)
    {
        text << "mov rdi, formatstr" << endl;
        text << "mov r8, [_last_value]" << endl;
        text << "sub r8, " << address << endl;
        text << "mov rsi, r8" << endl;
        text << "xor rax, rax" << endl;
        text << "call printf" << endl;
    }
    //Восстановление стека
    text << "mov rbp, [saved_stack]" << endl;
    text << "mov r8, rbp" << endl;
    text << "mov r9, [saved_stack + 8]" << endl;
    text << "sub r8, r9" << endl;
    text << "add r8, 16" << endl;
    text << "mov r10, saved_stack" << endl;
    text << "add r10, r8" << endl;
    text << "retur" << o << ":"
         << " cmp r8, 16" << endl;
    text << "je outt" << o << endl;
    text << "sub rsp, 2" << endl;
    text << "sub r10, 2" << endl;
    text << "mov r11w, [r10]" << endl;
    text << "mov [rsp], r11w" << endl;
    text << "sub r8, 2" << endl;
    text << "jmp retur" << o << endl;
    text << "outt" << o << ":" << endl;
    o++;
}
