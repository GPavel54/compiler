#include "Lexer.h"

Lexer::Lexer()
{
    
}

void Lexer::split(const string &s, char delim, vector<string> &elems)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }
}

void Lexer::getType(string &word, string &type)
{
    for (auto i = expressions_.begin(); i < expressions_.end(); i++)
    {
        if (regex_match(word.c_str(), i->expression))
        {
            type = i->name;
            return;
        }
    }
    type = "Wrong type";
}

void Lexer::makeTable(string path)
{
    ifstream in;
    in.open(path, ifstream::in);
    if (!in.is_open())
    {
        throw (Open_exception(path));
    }
    string buffer;
    initializeMap();
    int line = 1;
    int toNumirate = 0; // Для какого количества токенов записана позиция
    vector<string> outTokens;
    while (getline(in, buffer))
    {
        vector<string> tmp; // хранит в себе разбитую по пробелам строку
        split(buffer, ' ', tmp);
        checkStringLiterals(tmp);

        for (auto i = tmp.begin(); i < tmp.end(); i++)
        {
            eraseFreeSpace(*i);
            for (auto j = expressions_.begin(); j < expressions_.end(); j++)
            {
                smatch m;
                if (i->empty())
                {
                    break;
                }
                while (regex_search(*i, m, j->expression))
                {
                    if (j->name == "Comment")
                    {
                        tmp.clear();
                        *i = "";
                        break;
                    }
                    Token tmp = {line, 0, j->name, m.str()};
                    tokens_.push_back(tmp);
                    *i = m.suffix().str();
                    j = expressions_.begin();
                }
            }
            if (!i->empty())
            {
                throw(Wseq_exception(*i, line, in));
            }
        }
        int offset = 0;
        auto offsetIter = tokens_.begin();
        int inc = 0;
        while (inc < toNumirate)
        {
            inc++;
            offsetIter++;
        }
        for (; offsetIter != tokens_.end(); offsetIter++)
        {
            numirateToken(offsetIter->token, buffer, offset, offsetIter->col);
        }
        toNumirate += tokens_.size() - toNumirate;
        line++;
    }
    in.close();
}

void Lexer::printTable()
{
    int h = 0;
    for (auto i:tokens_)
    {
        h++;
        cout << setw(3) << h << " : token (" << setw(17) << i.name << ") = " << setw(12) << i.token << " col = " << setw(2) << i.col << " row = " << i.row << endl;
    }
}

void Lexer::initializeMap()
{
    expre tmp;

    tmp.name = "Comment";
    tmp.expression = "^//[a-zA-Z0-9 ]*";
    expressions_.push_back(tmp);

    tmp.name = "Symbol";
    tmp.expression = "'[\\S]'";
    expressions_.push_back(tmp);

    tmp.name = "public";
    tmp.expression = "^public$";
    expressions_.push_back(tmp);

    tmp.name = "static";
    tmp.expression = "^static$";
    expressions_.push_back(tmp);

    tmp.name = "Type";
    tmp.expression = "^(void)$|^(char)$|^(int)$";
    expressions_.push_back(tmp);

    tmp.name = "if";
    tmp.expression = "^if";
    expressions_.push_back(tmp);

    tmp.name = "main";
    tmp.expression = "^Main";
    expressions_.push_back(tmp);

    tmp.name = "while";
    tmp.expression = "^while";
    expressions_.push_back(tmp);

    tmp.name = "else";
    tmp.expression = "^else";
    expressions_.push_back(tmp);

    tmp.name = "return";
    tmp.expression = "^return$";
    expressions_.push_back(tmp);

    tmp.name = "break";
    tmp.expression = "^break";
    expressions_.push_back(tmp);

    tmp.name = ",";
    tmp.expression = "^,";
    expressions_.push_back(tmp);

    tmp.name = "Class";
    tmp.expression = "^class$";
    expressions_.push_back(tmp);

    tmp.name = "Left brace";
    tmp.expression = "^\\{";
    expressions_.push_back(tmp);

    tmp.name = "Right brace";
    tmp.expression = "^\\}";
    expressions_.push_back(tmp);

    tmp.name = "Operator .";
    tmp.expression = "^\\.";
    expressions_.push_back(tmp);

    tmp.name = "Left parenthesis";
    tmp.expression = "^\\(";
    expressions_.push_back(tmp);

    tmp.name = "Right parenthesis";
    tmp.expression = "^\\)";
    expressions_.push_back(tmp);

    tmp.name = "String literal";
    tmp.expression = "^\"[a-zA-Z0-9\\s+-=! _]*\"";
    expressions_.push_back(tmp);

    tmp.name = "Identifier";
    tmp.expression = "^[_|a-zA-Z][a-zA-Z0-9]*";
    expressions_.push_back(tmp);

    tmp.name = "Logical operator";
    tmp.expression = "^(==)|^(>=)|^(<=)|^<|^>|^(!=)";
    expressions_.push_back(tmp);

    tmp.name = "Operator =";
    tmp.expression = "^=";
    expressions_.push_back(tmp);
    
    tmp.name = "Operator 1";
    tmp.expression = "^\\*|^/";
    expressions_.push_back(tmp);

    tmp.name = "Operator 2";
    tmp.expression = "^\\-|^\\+";
    expressions_.push_back(tmp);

    tmp.name = "Left index";
    tmp.expression = "\\[";
    expressions_.push_back(tmp);
    
    tmp.name = "Constant";
    tmp.expression = "^[0-9]+";
    expressions_.push_back(tmp);

    tmp.name = "Right index";
    tmp.expression = "\\]";
    expressions_.push_back(tmp);

    tmp.name = "Semi";
    tmp.expression = "^;";
    expressions_.push_back(tmp);

}