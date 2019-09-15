#include "SyntaxTree.h"

SyntaxTree::SyntaxTree(Lexer &lexer)
{
    tokens = lexer.tokens_;
}

void SyntaxTree::makeSyntaxTree()
{
    auto i = tokens.begin();
    if (i->name == "Class")
    {
        root.reset(new treeNode(NULL, "Program", 2));
        treeNode *c = root.get();
    }
    while (i->name != "main")
    {
        i++;
        continue;
    }
    for (; i != tokens.end();)
    {
        treeNode *currentNode = getNextNode();
        if (i->name == "main")
        {
            currentNode->name = "Function main";
            currentNode->size = 1;
            currentNode->childrens.push_back(new treeNode(currentNode, "", 0));
        }
        else if (i->name == "Left brace")
        {
            currentNode->name = "Basic block";
        }
        else if (i->name == "Type")
        {
            vector<Token> expr;
            while (i->token != ";")
            {
                expr.push_back(*i);
                i++;
            }
            treeNode *ret = separateExpression(expr);
            currentNode->size++;
            currentNode->childrens.push_back(ret);
        }
        i++;
    }
}

SyntaxTree::treeNode *SyntaxTree::getNextNode()
{
    treeNode *out = root.get();
    int nodeToCheck = 1;
    int flag = 0;
    while (out->size != 0)
    {
        int childNum = -1;
        for (int i = 0; i < out->size; i++)
        {
            flag = 0;
            for (int j = 0; j < filled.size(); j++)
            {
                if (out->childrens[i]->id == filled[j])
                {
                    flag = 1;
                }
                if (flag == 1 && j == filled.size() - 1)
                {
                    flag = 2;
                }
            }
            if (flag == 0)
            {
                childNum = i;
                break;
            }
        }
        if (flag == 2)
        {
            out = out->parent->childrens[nodeToCheck++];
            continue;
        }
        out = out->childrens[childNum];
    }
    return out;
}

SyntaxTree::treeNode *SyntaxTree::getNextNode(treeNode *root, vector<int> f)
{
    int childNum = -1;
    for (int i = 0; i < root->size; i++)
    {
        bool flag = true;
        for (int j = 0; j < f.size(); j++)
        {
            if (root->childrens[i]->id == f[j])
            {
                flag = false;
            }
        }
        if (flag == true)
        {
            childNum = i;
            break;
        }
    }
    return root->childrens[childNum];
}

void SyntaxTree::addChildren(treeNode *p)
{
    p->parent->size++;
    p->parent->childrens.push_back(new treeNode(p->parent, "", 0));
}

SyntaxTree::treeNode *SyntaxTree::separateExpression(vector<Token> expression)
{
    vector<int> filledNodes;
    treeNode *root = new treeNode(NULL, "Statement", 1);
    for (auto i = expression.begin(); i != expression.end(); i++)
    {
        treeNode *node = getNextNode(root, filledNodes);
        if (i->name == "Type")
        {
            bool declaration = true;
            for (auto j = expression.begin(); j != expression.end(); j++)
            {
                if (j->name == "Operator =")
                {
                    declaration = false;
                }
            }
            if (declaration)
            {
                node->size = 1;
                node->name = "Declaration " + i->token;
                string name = "";
                for (auto j = i + 1; j != expression.end(); j++)
                {
                    name += j->token; 
                }
                node->childrens.push_back(new treeNode(node, name, 0));
                filledNodes.push_back(node->childrens[0]->id);
                filledNodes.push_back(node->id);
            }
            else
            {

            }
        }
    }
    filled.insert(filled.end(), filledNodes.begin(), filledNodes.end());
    return root;
}