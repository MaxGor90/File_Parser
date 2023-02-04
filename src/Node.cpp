#include "Node.h"
#include <fstream>
#include <sstream>


Node::Node(unsigned int id, const std::string& name, Node_Ptr parent): 
    Id {id}, Name {name}, Parent {parent}
{
    if (Parent)
        Level = Parent->getLevel() +1;
}


// Для корневого узла идентификатор родителя выбрано отображать как "0", 
// т.к. требований на этот счёт в задании не дано
void ValueNode::print(std::ofstream& out)
{
    std::stringstream sStream;
    std::string Indentation { std::string(2*Level, ' ') };
    
    sStream << Indentation + '(';
    sStream << Id << ", " 
            << ((Parent == nullptr)? 0 : Parent->getId()) << ", "
            << Name << ", "
            << Value << ')';
    std::string str {sStream.str()};
    out << str << std::endl;
}

void ListNode::print(std::ofstream& out)
{
    std::stringstream sStream;
    std::string Indentation { std::string(2*Level, ' ') };

    sStream << Indentation + '(';
    sStream << Id << ", " 
            << ((Parent == nullptr)? 0 : Parent->getId()) << ", "
            << Name;
    for (auto child : Children)
        sStream << ", " << child->getId();
    
    sStream << ')';
    std::string str {sStream.str()};
    out << str << std::endl;

    for (auto child : Children)
        child->print(out);
}


/*---------------------------------------------------------------------------*/
void ValueNode::addChild(Node_Ptr node) 
{
    throw std::string("Function addChild must never be used by ValueNode object");
    return;
}

void ListNode::setValue(const std::string& value) 
{
    throw std::string("Function setValue must never be used by ListNode object");
    return;
}

std::string ListNode::getValue() const 
{
    throw std::string("Function getValue must never be used by ListNode object");
    return "";
}

