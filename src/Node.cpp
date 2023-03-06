#include "Node.h"
#include <fstream>
#include <sstream>
#include <iostream>


Node::Node(unsigned int id, const std::string& name, Node* parent): 
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

void ValueNode::writeToDB(DB* db)
{
    std::stringstream sStream;
    sStream << '(' << Id << ',' << Parent->getId() << ",\'" << Name << "\',\'" << Value << "\')";
    try {
        db->work->exec0(
            "INSERT INTO shape_params(id, parent_id, name, value) VALUES " + sStream.str() + ';' );
    } catch (const std::exception& e)
    {
        std::cerr << e.what();
        return;
    }
}

void ListNode::print(std::ofstream& out)
{
    std::stringstream sStream;
    std::string Indentation { std::string(2*Level, ' ') };

    sStream << Indentation + '(';
    sStream << Id << ", " 
            << ((Parent == nullptr)? 0 : Parent->getId()) << ", "
            << Name;
    for (auto& child : Children)
        sStream << ", " << child->getId();
    
    sStream << ')';
    std::string str {sStream.str()};
    out << str << std::endl;

    for (auto& child : Children)
        child->print(out);
}

void ListNode::writeToDB(DB* db)
{
    std::stringstream sStream;

    sStream << '(' << Id << ',';
    if (Parent)
        sStream << Parent->getId();
    else sStream << "NULL";
    sStream << ",\'" << Name << "\'," << "NULL" << ')';
    try {
        db->work->exec0(
            "INSERT INTO shape_params(id, parent_id, name, value) VALUES " + sStream.str() + ';' );
    } catch (const std::exception& e)
    {
        std::cerr << e.what();
        return;
    }
    for (auto& child : Children)
        child->writeToDB(db);
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

