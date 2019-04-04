#include "Lexer.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Using " << argv[0] << " infile.cs outfile" << endl;
        return 1;
    }

    ifstream in;
    in.open(argv[1], ifstream::in);
    if (!in.is_open())
    {
        cout << "Error with opening file " << argv[1] << endl;
        return 1;
    }
    string buffer;
    Location loc = {0,0};
    Lexer lex;
    lex.initializeMap();
    while (getline(in, buffer))
    {
        vector<string> elements;
        lex.split(buffer, ' ', elements);
        for (auto i = elements.begin(); i < elements.end(); i++)
        {
            eraseFreeSpace(*i);
            if (i->empty())
            {
                elements.erase(i);
                continue;
            }
            string type;
            lex.getType(*i, type);
            cout << *i << " <- " << type << endl; 
        }
        
        loc.line++;
    }
}
