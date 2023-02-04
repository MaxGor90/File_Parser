#ifndef H_NODE
#define H_NODE

#include <string>
#include <vector>
#include <memory>


class Node
{
public:
    using Node_Ptr = std::unique_ptr<Node>;
    
    Node (unsigned int id, const std::string& name, Node* parent = nullptr);
    virtual ~Node() {}

protected:    
    unsigned int Id;
    std::string Name;
    Node* Parent {nullptr};
    unsigned int Level{0};

public:
    virtual unsigned int getLevel() const { return Level; }
    virtual unsigned int getId() const { return Id; }
    virtual void addChild(Node_Ptr node) = 0;
    virtual void setValue(const std::string& value) = 0;
    virtual std::string getValue() const = 0;
    virtual void print(std::ofstream& out) = 0;

};

// Node with value
class ValueNode : public Node
{
    std::string Value;

public:
    ValueNode(unsigned int id, const std::string& name, Node* parent = nullptr) :
        Node(id, name, parent)
    {}
    ~ValueNode() {}

    void addChild(Node_Ptr node) override;
    void setValue(const std::string& value) override { Value = value; };
    std::string getValue() const override { return Value; };
    void print(std::ofstream& out) override;
};

// Node with list of children
class ListNode : public Node
{
    std::vector<Node_Ptr> Children;

public:
    ListNode(unsigned int id, const std::string& name, Node* parent = nullptr) :
        Node(id, name, parent)
    {}
    ~ListNode() {}
    
    void addChild(Node_Ptr node) override { Children.push_back(std::move(node)); };
    void setValue(const std::string& value) override;
    std::string getValue() const override;
    void print(std::ofstream& out) override;
};


#endif /* H_NODE */
