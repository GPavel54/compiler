#include "addfunctions.h"

void eraseFreeSpace(string& str)
{
    for (auto i = str.begin(); i < str.end(); i++)
    {
        if (*i == ' ' || *i == '\t')
        {
            str.erase(i);
        }
    }
}