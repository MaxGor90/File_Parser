#include "Tree.h"
#include "IdGenerator.h"
#include <algorithm>
#include <stdexcept>



void Tree::Deserialize(File* file) 
{
    if (!file->in_file.is_open())
    {
        throw std::string("Unable to read the file");
        return;
    }
    
    try {
        Root = ParseNode(file->in_file); 
    }
    catch(const std::string& exception) {
        throw exception;
        return;
    }
    
    // Проверка на наличие в файле видимых символов после закрывающей корневой узел скобки
    // Это может означать наличие более одного корневого узла, нарушение структуры данных, либо просто мусор
    // В любом случае выдаём ошибку формата данных
    std::string check;
    while (getline(file->in_file, check))
    {
        check.erase(std::remove_if( check.begin(), 
                                    check.end(), 
                                    [](unsigned char ch) { return   (ch == ' ')  || 
                                                                    (ch == '\t') ||
                                                                    (ch == '\n') ||
                                                                    (ch == '\r'); }),
                    check.end());
        if (!check.empty())
        {
            throw std::string("Invalid data format");
            return;
        }
    }
}

void Tree::Serialize(File* file) 
{
    if (!file->out_file.is_open())
    {
        throw std::string("Unable to write into the file");
        return;
    }
    Root->print(file->out_file);
    file->out_file.close();
}

Node::Node_Ptr Tree::ParseNode(std::ifstream& inStream, Node::Node_Ptr parent) 
{
    std::string str;
    std::getline(inStream, str);
    
    // Допустим возможность наличия отступов
    trim(str);
    
    // Закрывающая лист скобка - первый значимый символ
    if (str.substr(0,1) == "}")
        return nullptr;
        
    // Скобка, закрывающая лист, содержащий другой лист, не на новой строке - нарушение формата
    if (str.find("}}") != std::string::npos)
    {
        throw std::string("Invalid data format");
        return nullptr;
    }

    // На строке, не содержащей первым значимым символом закрывающую лист скобку, нет "=" - нарушение формата
    if (str.find('=') == std::string::npos)
    {
        throw std::string("Invalid data format");
        return nullptr;
    }

    std::string Type = str.substr(str.find('=') +1);
    
    // Допустим возможность отсутствия пробелов вокруг "=", не рассчитываем на наличие прорбела
    trim(Type);
    
    Type = Type.substr(0, 1);
    std::string value{'\"'};
    std::string list{"{"};
    
    // Узел-лист
    if (Type == list)
    {
        // Лист описан одной строкой
        if (str.find('}') != std::string::npos)
        {
            return ParseListNode(str, parent);
        }
        
        // Лист не описан одной строкой, но после '{' есть значимые символы - нарушение формата
        str.erase(  std::remove_if( str.begin(), 
                                    str.end(), 
                                    [](unsigned char ch) { return   (ch == ' ')  || 
                                                                    (ch == '\t') ||
                                                                    (ch == '\n') ||
                                                                    (ch == '\r'); }),
                    str.end());
        if (str[str.size() -1] != '{')
        {
            throw std::string("Invalid data format");
            return nullptr;
        }

        std::string Name {getName(str)};
        std::shared_ptr<ListNode> node = std::shared_ptr<ListNode>(new ListNode(IdGenerator::generateId(), Name, parent));
        while (true)
        {
            Node::Node_Ptr newNode { ParseNode(inStream, node)};
            if (newNode == nullptr)
                break;
            node->addChild(newNode);
        }
        return node;
    }
    // Узел со значением
    else if (Type == value)
    {
        return ParseValueNode(str, parent);
    }
    else
    {
        throw std::string("Invalid data format");
        return nullptr;
    }
}


Node::Node_Ptr Tree::ParseValueNode(const std::string& str, Node::Node_Ptr parent) 
{
    if (str.find('=') == std::string::npos)
        return nullptr;
    std::string Name = getName(str);
    std::shared_ptr<ValueNode> node = std::shared_ptr<ValueNode>(new ValueNode(IdGenerator::generateId(), Name, parent));
    std::string value;
    
    try {
        value = getValue(str);
    } 
    catch (const std::string& except) {
        throw except;
        return nullptr;
    }
    
    node->setValue(value);
    return node;
}

Node::Node_Ptr Tree::ParseListNode(const std::string& str, Node::Node_Ptr parent) 
{
    std::string Name = getName(str);
    std::shared_ptr<ListNode> node = std::shared_ptr<ListNode>(new ListNode(IdGenerator::generateId(), Name, parent));
    std::string list {str.substr(str.find('{') +1, 
                                 str.find('}') - str.find('{') -1)};
    
    // Вложенный лист на одной строке - нарушение формата
    if (list.find('{') != std::string::npos)
    {
        throw std::string("Invalid data format");
        return nullptr;
    }
    
    while (true)
    {
        Node::Node_Ptr newNode = ParseValueNode(list, node);
        if (newNode == nullptr)
            break;
        node->addChild(newNode);
        list = list.substr(list.find('\"') +1);
        list = list.substr(list.find('\"') +1);
        if (list.empty())
            break;
    }

    return node;
}

std::string Tree::getValue(const std::string& string) 
{
    std::string value {string.substr(string.find('=') +1)};
    trim(value);
    
    // Между '=' и '\"' лишние символы
    if (value[0] != '\"')
    {
        throw std::string("Invalid data format");
        return "";
    }
    value = value.substr(value.find('\"') +1);
    
    // Нет закрывающей кавычки в данной строке - признак нарушения структуры, наличия в значении символа \n либо лишнего символа кавычек
    if (value.find('\"') == std::string::npos) 
    {
        throw std::string("Invalid data format");
        return "";
    }
    value = value.substr(0, value.find('\"'));

    return value; 
}

void Tree::trim(std::string& string) 
{
    while (string[0]==' ')
        string.erase(string.begin(), string.begin() +1);

    while (string[string.size() -1] == ' ')
        string.erase(string.end() -1, string.end());
}


std::string Tree::getName(const std::string& string) 
{
    if (string.find('=') == std::string::npos)
    {
        throw std::string("Invalid data format");
        return "";
    }
    std::string name { string.substr(0, string.find('=')) };

    trim(name);
    
    // Имя пустое, начинается с цифры, либо содержит пробел - нарушение формата
    if ( name.empty()                               || 
        ((int)name[0] >= 48 && (int)name[0] <= 57)  ||
        (name.find(' ') != std::string::npos) )
    {
        throw std::string("Invalid data format");
        return "";
    }

    return name;
}


