#ifndef H_TREE
#define H_TREE

#include "Node.h"
#include "File.h"



class Tree
{
    Node::Node_Ptr Root {nullptr};

public:

    void Deserialize(File* file);
    void Serialize(File* file);
    Node::Node_Ptr ParseNode(std::ifstream& inStream, Node* parent = nullptr);
    Node::Node_Ptr ParseValueNode(const std::string& str, Node* parent = nullptr);
    Node::Node_Ptr ParseListNode(const std::string& str, Node* parent = nullptr);
    std::string getName(const std::string& string);
    std::string getValue(const std::string& string);
    static inline void trim(std::string& string);
};


#endif /* H_TREE */
