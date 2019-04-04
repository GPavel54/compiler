#include "Lexer.h"

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
    for (auto i = expressions.begin(); i < expressions.end(); i++)
    {
        if (regex_match(word.c_str(), i->expression))
        {
            type = i->name;
            return;
        }
    }
    type = "Wrong type";
}

void Lexer::initializeMap()
{
    exp tmp;

    tmp.name = "key word using";
    tmp.expression = "[;|^]?using$";
    expressions.push_back(tmp);

    tmp.name = "int";
    tmp.expression = "[;|^]?int$";
    expressions.push_back(tmp);

    tmp.name = "public";
    tmp.expression = "[;|^]?public$";
    expressions.push_back(tmp);

    tmp.name = "void";
    tmp.expression = "[;|^]?void$";
    expressions.push_back(tmp);

    tmp.name = "Key word class";
    tmp.expression = "[;|^]?class$";
    expressions.push_back(tmp);

    tmp.name = "Key word class";
    tmp.expression = "[;|^]?class$";
    expressions.push_back(tmp);

    tmp.name = "Left brace";
    tmp.expression = "[;|^]?\{$";
    expressions.push_back(tmp);

    // tmp.name = "Right brace";
    // tmp.expression = "[;|^]?}$";
    // expressions.push_back(tmp);

    tmp.name = "Identifier";
    tmp.expression = "[_|a-zA-Z][a-zA-Z0-9]+";
    expressions.push_back(tmp);
}