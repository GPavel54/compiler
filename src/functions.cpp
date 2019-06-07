#include "addfunctions.h"

void eraseFreeSpace(string& str)
{
    if (str.find("\"") != string::npos)
    {
        return;
    }
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

void checkStringLiterals(vector<string>& buffer)
{
    for (auto i = buffer.begin(); i < buffer.end(); i++)
    {
        if (i->find("\"") != string::npos)
        {
            int n = 0, p = 0;
            while ( (p = i->find("\"", p)) != string::npos ) 
            {
                ++n;
                ++p;
            } 
            if (n == 2)
            {
                break;
            }
            else if (n == 1)
            {
                if (i + 1 != buffer.end())
                {
                    if ((++i)->find("\"") != string::npos)
                    {
                        *(i - 1) += " " + *i;
                        buffer.erase(i);
                    }
                }
            }
        }
    }
}