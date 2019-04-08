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
        throw (Open_exception());
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
        for (auto j = tokens_.begin() + toNumirate; j < tokens_.end(); j++)
        {
            numirateToken(j->token, buffer, offset, j->col);
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
        cout << setw(2) << h << " : token (" << setw(17) << i.name << ") = " << setw(12) << i.token << " col = " << setw(2) << i.col << " row = " << i.row << endl;
    }
}

void Lexer::initializeMap()
{
    exp tmp;

    tmp.name = "Comment";
    tmp.expression = "^//[a-zA-Z0-9 ]+";
    expressions_.push_back(tmp);

    tmp.name = "int";
    tmp.expression = "^int$";
    expressions_.push_back(tmp);

    tmp.name = "public";
    tmp.expression = "^public$";
    expressions_.push_back(tmp);

    tmp.name = "void";
    tmp.expression = "^void$";
    expressions_.push_back(tmp);

    tmp.name = "if";
    tmp.expression = "^if";
    expressions_.push_back(tmp);

    tmp.name = "while";
    tmp.expression = "^while";
    expressions_.push_back(tmp);

    tmp.name = "else";
    tmp.expression = "^else";
    expressions_.push_back(tmp);

    tmp.name = "char";
    tmp.expression = "^char$";
    expressions_.push_back(tmp);

    tmp.name = ",";
    tmp.expression = "^,";
    expressions_.push_back(tmp);

    tmp.name = "Key word class";
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
    tmp.expression = "^\"[a-zA-Z0-9 +-=]*\"";
    expressions_.push_back(tmp);

    tmp.name = "Identifier";
    tmp.expression = "^[_|a-zA-Z][a-zA-Z0-9]*";
    expressions_.push_back(tmp);

    tmp.name = "Operator";
    tmp.expression = "^[==|=|*|\\-|+|/|<|>]";
    expressions_.push_back(tmp);

    tmp.name = "Semi";
    tmp.expression = "^;";
    expressions_.push_back(tmp);

    tmp.name = "Constant";
    tmp.expression = "^[0-9]+";
    expressions_.push_back(tmp);
}