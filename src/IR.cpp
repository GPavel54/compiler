#include "IR.h"

IR::IR(Lexer &lexer)
{
    tokens = lexer.tokens_;
    sp = 0;
    reserved_memory = 0;
    memory_counter = 0;
    nbreak.needBreak = false;
    nbreak.num = 0;
    t = 0;
    remember = false;
    remembered_variable1.clear();
    remembered_variable2.clear();
    block_number = 0;
}

void IR::generateIR()
{
    separateToFunctions();
    createSymbolicTable(*(functions.begin()));
    separateFunc(*(functions.begin()));

    ofstream outfile;
    cout << text.str() << endl;
    cout << "--------------" << endl;
    sort(symbolicTable.begin(), symbolicTable.end(), greater<VarInfo>);
    for (auto i = symbolicTable.begin(); i != symbolicTable.end(); i++)
    {
        cout << "var - " << i->name << " type - " << i->type << " block - " << i->block << endl;
    }
}

void IR::separateToFunctions()
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

void IR::separateFunc(list<Token> &func)
{
    int level = 0;
    int bp = 0;
    text << "\t%bp = %sp" << endl;
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
            addVariable((cmp ? 2 : 4) * size, name, type.token, level, block_number);
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
                            text << "\t%sp = " << "byte " << "*j" << endl;
                            shift = 2;
                        }
                        else
                        {
                            text << "\t[%sp + " << shift++ << "] = " << "byte " << "*j" << endl;
                        }
                    }
                    text << "\t[%sp + " << shift++ << "] = byte 0" << endl;
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
            block_number++;
        }
        else if (i->name == "Right brace") // закрытие блока
        {
            int addToSP = 0;
            block_number--;
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
                text << "\t%sp = " << addToSP << "; free memory after left code block" << endl;
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
            else if (nextBlock.type == "while")
            {
                text << ";While comparison below" << endl;
                text << nextBlock.toPaste << endl;
                if (nbreak.needBreak == true)
                {
                    nbreak.needBreak = false;
                    text << "break" << nbreak.num << ":" << endl;
                }
            }
        }
        else if (i->name == "Identifier")
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
                    text << "\tt" << t++ << " = %bp" << endl;
                    text << "\tt" << t++ << " = t" << t - 2 << " - " << hmap.find(toPrint->token)->second.address << endl;
                    text << "\txor %ax %ax";
                    if (hmap.find(toPrint->token)->second.type == "char" && hmap.find(toPrint->token)->second.size == 2)
                    {
                        text << "\t%axw = [t" << t - 2 << "]" << endl;
                        text << "call print" << endl;
                    }
                    else if (hmap.find(toPrint->token)->second.type == "char" && hmap.find(toPrint->token)->second.size != 2)
                    {
                        text << "\t%axq = [t" << t - 2 << "]" << endl;
                        text << "call print" << endl;
                    }
                    else
                    {
                        text << "\t %axd = [t" << t - 2 << "]" << endl;
                        text << "call print" << endl;
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

                    text << "\tt" << t++ << " = %bp - " << hmap.find(toPrint->token)->second.address << endl;
                    text << "\tt" << t++ << " = t" << t-2 << endl;
                    text << "\tshr t" << t - 3 << " 32" << endl;
                    text << "\tshl t" << t - 3 << " 32" << endl;
                    text << "\t t" << t - 2 << "d = %ax" << endl;
                    text << "\tor t" << t - 2 << " t" << t - 3 << endl; 

                    if (hmap.find(toPrint->token)->second.type == "char")
                    {
                        text << "call print" << endl;
                    }
                    else
                    {
                        text << "call print" << endl;
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
                translateToRpn(arrExpr);
                processExpr(*ident, arrExpr, -2);
            }
        }
        else if (i->name == "if") // обработка условия
        {
            //Обработка значения слева от знака
            static int num = 0;
            i++;
            auto first = ++i;
            text << ";Compare two values" << endl
                 << "\t%ax = %bp" << endl;
            if (first->name == "Constant")
            {
                text << "\tt" << t++ << " = " << i->token << endl;
            }
            else if (first->name == "Identifier" && (++i)->name != "Left index")
            {
                text << "\t%ax = %ax - " << hmap.find(first->token)->second.address << endl;
                if (hmap.find(first->token)->second.type == "int")
                {
                    text << "\tt" << t++ << " = [%ax]" << endl;
                }
                else
                {
                    text << "t" << t++ << "b, [%ax]" << endl;
                }
            }
            else
            {
                vector<Token> expr;
                while ((i++)->name != "Right index")
                {
                    expr.push_back(*i);
                }
                text << "beforeget:" << endl;
                getArrayValue(first->token, expr);
                text << "afterget:" << endl;
                text << "\tt" << t++ << "d = [%sp]" << endl;
                text << "\t%sp = %sp + 4" << endl;
                // todo
            }
            auto sign = i++;
            auto second = i;
            if (second->name == "Constant")
            {
                text << "\tt" << t++ << "d = " << second->token << endl;
            }
            else if (second->name == "Identifier" && (++i)->name != "Left index")
            {
                text << "\t%ax = " << hmap.find(second->token)->second.address << endl;
                if (hmap.find(second->token)->second.type == "int")
                {
                    text << "\tt" << t++ << "d, [%ax]" << endl;
                }
                else
                {
                    text << "\tt" << t++ << "b, [%ax]" << endl;
                }
            }
            else
            {
                vector<Token> expr;
                while ((i++)->name != "Right index")
                {
                    expr.push_back(*i);
                }
                //todo
            }
            text << "\tcmp t" << t - 3 << "d t " << t -2  << "d" << endl;
            if (sign->token == "==")
            {
                text << "\tbne if" << num << endl;
            }
            else if (sign->token == ">")
            {
                text << "\tble if" << num << endl;
            }
            else if (sign->token == "<")
            {
                text << "\tbge if" << num << endl;
            }
            else if (sign->token == ">=")
            {
                text << "\tbl if" << num << endl;
            }
            else if (sign->token == "<=")
            {
                text << "\tbg if" << num << endl;
            }
            else // !=
            {
                text << "\tbe if" << num << endl;
            }
            nextBlock.type = "if";
            nextBlock.num = num++;
        }
        else if (i->name == "while")
        {
            static int o = 0;
            i++;
            auto first = ++i;
            nextBlock.type = "while";
            nextBlock.num = o;
            stringstream preparedCode;
            preparedCode << ";Compare two values" << endl
                 << "\t%ax = %bp" << endl;
            if (first->name == "Constant")
            {
                preparedCode << "\tt" << t++ << "d = " << i->token << endl;
            }
            else if (first->name == "Identifier" && (++i)->name != "Left index")
            {
                preparedCode << "\t%ax = %ax - " << hmap.find(first->token)->second.address << endl;
                if (hmap.find(first->token)->second.type == "int")
                {
                    preparedCode << "t" << t - 2 << "d = [%ax]" << endl;
                }
                else
                {
                    preparedCode << "t" << t - 2 << "b = [%ax]" << endl;
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
            }
            auto sign = i++;
            auto second = i;
            if (second->name == "Constant")
            {
                preparedCode << "t" << t - 2 << "d = " << second->token << endl;
            }
            else if (second->name == "Identifier" && (++i)->name != "Left index")
            {
                preparedCode << "%ax = %ax - " << hmap.find(second->token)->second.address << endl;
                if (hmap.find(second->token)->second.type == "int")
                {
                    preparedCode << "t" << t - 1 << "d = [%ax]" << endl;
                }
                else
                {
                    preparedCode << "t" << t - 1 << "b, [%ax]" << endl;
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
            preparedCode << "cmp t" << t - 1 << "d t" << t - 2 << "d" << endl;
            if (sign->token == "==")
            {
                preparedCode << "be while" << o << endl;
            }
            else if (sign->token == ">")
            {
                preparedCode << "bge while" << o << endl;
            }
            else if (sign->token == "<")
            {
                preparedCode << "ble while" << o << endl;
            }
            else if (sign->token == ">=")
            {
                preparedCode << "bg while" << o << endl;
            }
            else if (sign->token == "<=")
            {
                preparedCode << "bl while" << o << endl;
            }
            else // !=
            {
                preparedCode << "bne while" << o << endl;
            }
            text << "while" << o << ":" << endl;
            nextBlock.toPaste = preparedCode.str();
            o++;
        }
        else if (i->name == "break")
        {
            static int br_n = 0;
            if (nbreak.needBreak == true)
            {
                continue;
            }
            nbreak.needBreak = true;
            nbreak.num = br_n;
            text << "bmp break" << br_n << endl;
            br_n++;
        }
    }
}

/*
 * В эту функцию передается выражение,  
 * переведенное в обратную польскую запись
*/
void IR::processExpr(Token left, vector<Token> &expression, int shift)
{
    remember = false;
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
                if (!remember)
                {
                    if (remembered_variable1 == "")
                    {
                        remembered_variable1 = i->token;    
                    }
                    else
                    {
                        remembered_variable2 = i->token;
                        remember = true;
                    }
                }
                else
                {
                    remembered_variable1 = i->token;
                }
            }
        }
        else if (i->name == "Constant" || i->name == "Symbol")
        {
            if (!remember)
            {
                if (remembered_variable1 == "")
                {
                    remembered_variable1 = i->token;    
                }
                else
                {
                    remembered_variable2 = i->token;
                    remember = true;
                }
            }
            else
            {
                remembered_variable1 = i->token;
            }

        }
        else if (i->name == "Operator 1" || i->name == "Operator 2")
        {
            if (!remember)
            {
                text << "\tt" << t++ << " = " << remembered_variable1 << " " << i->token << " " << remembered_variable2 << endl;
            }
            else
            {
                text << "\tt" << t << " = " << "t" << t - 1 << " " << i->token << " " << remembered_variable1 << endl;
                t++;
            }
        }
    }
    /**
     * На вершине стека находится значение вычисленного выражения.
     *  Необходимо записать его в заданную переменную
    */
    //Перенесли значение выражение в регистр
    int rmt = t - 1;
    if (shift == -1) //Значит необходимо вычислить смещение от начала массива
    {
        if (hmap.find(left.token)->second.type == "char")
        {
            text << "\tt" << t++ << " = " << t << " * 2" << endl;
        }
        else
        {
            text << "\tt" << t++ << " = " << t << " * 4" << endl;
        }
        return;
    }

    //Сместились к этой переменной
    text << "\tt" << t++ << " = %bp" << endl;
    text << "\tt" << t++ << " = t" << t - 2 << " - " << hmap.find(left.token)->second.address << endl;

    if (shift != 0)
    {
        text << "\tt" << t - 1 << " = " << "t" << rmt << " + " << shift << endl;
    }
    if (hmap.find(left.token)->second.type == "char")
    {
        text << "\t[t" << t << "] = t" << rmt << "w" << endl;
    }
    else
    {
        text << "\t[t" << t << "] = t" << rmt << "d" << endl;
    }
}

/**
 * Обработка случая, если в индекс массива вложен другой индекс
 * после ее исполнения на вершине стека окажется 4х байтное необходимое значение
*/
void IR::getArrayValue(string &name, vector<Token> expression)
{
    for (auto i = expression.begin(); i != expression.end(); i++)
    {
        if (i->name == "Constant")
        {
            // text << "push dword " << i->token << endl;
            if (!remember)
            {
                if (remembered_variable1 == "")
                {
                    remembered_variable1 = i->token;    
                }
                else
                {
                    remembered_variable2 = i->token;
                    remember = true;
                }
            }
            else
            {
                remembered_variable1 = i->token;
            }
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
                if (!remember)
                {
                    if (remembered_variable1 == "")
                    {
                        remembered_variable1 = i->token;    
                    }
                    else
                    {
                        remembered_variable2 = i->token;
                        remember = true;
                    }
                }
                else
                {
                    remembered_variable1 = i->token;
                }
            }
        }
        else if (i->name == "Operator 1" || i->name == "Operator 2")
        {
            if (!remember)
            {
                text << "\tt" << t++ << " = " << remembered_variable1 << " " << i->token << " " << remembered_variable2 << endl;
            }
            else
            {
                text << "\tt" << t << " = " << "t" << t - 1 << " " << i->token << " " << remembered_variable1 << endl;
                t++;
            }
        }
    }
    /**
     * Смещение от начала массива вычисленно и хранится на вершине стека
     * теперь необходимо вместо этого смещения поместить нужное значение
    */
    if (hmap.find(name)->second.type == "char")
    {
        text << "\tt" << t++ << "d = t" << t - 2 << " * 2" << endl; 
    }
    else
    {
        text << "\tt" << t++ << "d = t" << t - 2 << " * 4" << endl;
    }
    text << "\tt" << t++ << " = %bp" << endl;
    text << "\tt" << t++ << " = t" << t << " - t" << t - 4 << endl;
    text << "\t[%sp] = t" << t << endl;
}

void IR::translateToRpn(vector<Token> &expression)
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

void IR::createSymbolicTable(list<Token> &func)
{
    vector<string> variables;
    for (auto i = func.begin(); i != func.end(); i++)
    {
        if (i->name == "Type")
        {
            variables.push_back((++i)->token);
        }
    }
}

void IR::addVariable(int size, Token &token, string &type, int level, int &bp)
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
    //text << "sub rsp, " << size << " ; allocate memory for var " << token.token << " level = " << level << endl; // выделение памяти путем смещения вершины стека
    text << "\t%sp = %sp - " << size << endl;
    memory_counter += size;
    if (memory_counter > reserved_memory)
    {
        reserved_memory = memory_counter;
    }
    symbolicTable.push_back({token.token, size, type, block_number});
}

void IR::printExpr(vector<Token> &expr)
{
    for (auto i = expr.begin(); i != expr.end(); i++)
    {
        cout << i->token << " ";
    }
    cout << endl;
}

void IR::printHashTable()
{
    int counter = 1;
    for (auto j = hmap.begin(); j != hmap.end(); j++)
    {
        cout << counter << ": " << j->first << " | " << j->second.size << " | " << j->second.type << endl;
    }
}

void IR::printVariable(IR::TYPE type, string address)
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