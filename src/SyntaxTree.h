#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H
#include "addfunctions.h"
#include "Lexer.h"

class SyntaxTree
{
    struct treeNode
    {
        int id;
        string name;
        string token;
        treeNode * parent;
        vector<treeNode *> childrens;
        int size;
        bool terminal;
        treeNode(treeNode * parentNode, string nodeName, int numberOfChildrens)
        {
            static int identifier = 0;
            id = identifier++; 
            name = nodeName;
            token = "";
            parent = parentNode;
            for (int i = 0; i < numberOfChildrens; i++)
            {
                childrens.push_back(new treeNode(this, "", 0));
            }
            size = numberOfChildrens;
            terminal = false;
        }
    };
    unique_ptr<treeNode> root;
    vector<int> filled;
    list<Token> tokens;
    void addChildren(treeNode * p);
    treeNode * separateExpression(vector<Token> expression);
    treeNode * getNextNode();
    treeNode * getNextNode(treeNode * root, vector<int> f);
public:
    void makeSyntaxTree();
    SyntaxTree(Lexer &lexer);
};

#endif