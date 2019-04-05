#include "addfunctions.h"

void eraseFreeSpace(string& str)
{
    for (auto i = str.begin(); i < str.end(); i++)
    {
        if (*i == ' ' || *i == '\t')
        {
            str.erase(i);
            i--;
        }
    }
}

void numirateToken(string& token, string& buffer, int offset, int& position)
{
    position = buffer.find(token, offset);
    offset += token.length();
}