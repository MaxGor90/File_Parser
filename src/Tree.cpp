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
        file->in_file.close();
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
            file->in_file.close();
            return;
        }
    }

    file->in_file.close();
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

void Tree::WriteToDB(DB* db) 
{
    Root->writeToDB(db);
}

Node::Node_Ptr Tree::ParseNode(std::ifstream& inStream, Node* parent) 
{
    std::string str;
    std::getline(inStream, str);
    
    // Допустим возможность наличия отступов
    trim(str);
    
    // Закрывающая лист скобка - первый значимый символ
    if (str.substr(0,1) == "}")
    {
        std::string check {str.substr(str.find('}') +1)};
        trim(check);
        
        // После закрывающей лист скобки в строке имеются видимые символы - нарушение формата
        if (!check.empty())
        {
            throw std::string("Invalid data format");
        }
        return nullptr;
    }
        
    // На строке, не содержащей первым значимым символом закрывающую лист скобку, нет "=" - нарушение формата
    if (str.find('=') == std::string::npos)
    {
        throw std::string("Invalid data format");
        return nullptr;
    }

    std::string Type = str.substr(str.find('=') +1);
    
    // Допустим возможность отсутствия пробелов вокруг "=", не рассчитываем на наличие пробела
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
            std::string check {str.substr(str.find('}') +1)};
            trim(check);
            
            // После закрывающей лист скобки в строке имеются видимые символы - нарушение формата
            if (!check.empty())
            {
                throw std::string("Invalid data format");
                return nullptr;
            }
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
        std::unique_ptr<ListNode> node = std::make_unique<ListNode>(IdGenerator::generateId(), Name, parent);
        while (true)
        {
            Node::Node_Ptr newNode { ParseNode(inStream, node.get())};
            if (newNode == nullptr)
                break;
            node->addChild(std::move(newNode));
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


Node::Node_Ptr Tree::ParseValueNode(const std::string& str, Node* parent) 
{
    if (str.find('=') == std::string::npos)
        return nullptr;
    std::string Name = getName(str);
    std::unique_ptr<ValueNode> node = std::make_unique<ValueNode>(IdGenerator::generateId(), Name, parent);
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

Node::Node_Ptr Tree::ParseListNode(const std::string& str, Node* parent) 
{
    std::string Name = getName(str);
    std::unique_ptr<ListNode> node = std::make_unique<ListNode>(IdGenerator::generateId(), Name, parent);
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
        Node::Node_Ptr newNode = ParseValueNode(list, node.get());
        if (newNode == nullptr)
            break;
        node->addChild(std::move(newNode));
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
    
    // Значение узла пустое либо содержит только пробелы
    std::string check {value};
    trim(check);
    if (check.empty())
    {
        throw std::string("Invalid data format");
        return "";
    }
    
    return value; 
}

void Tree::trim(std::string& string) 
{
    while ( string[0]==' ' || 
            string[0]=='\r' || 
            string[0]=='\t')
        string.erase(string.begin(), string.begin() +1);

    while ( string[string.size() -1] == ' ' || 
            string[string.size() -1] == '\r' ||
            string[string.size() -1] == '\t')
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
    
    try { 
        checkName(name); 
    }
    catch (const std::string& except)
    {
        throw except;
        return "";
    }
    
    return name;
}

void Tree::checkName(const std::string& name)
{
    // Имя пустое, начинается с цифры, либо содержит пробел - нарушение формата
    if ( name.empty()                               || 
        ((int)name[0] >= 48 && (int)name[0] <= 57)  ||
        (name.find(' ') != std::string::npos) )
    {
        throw std::string("Invalid data format");
        return;
    }

    // Имя содержит нехарактерные для названий символы: {}|\/+=-()?! и тд
    for (char ch : name)
    {
        if ( (int)ch < 48 ||
             ((int)ch > 57 && (int)ch < 65) || 
             ((int)ch > 90 && (int)ch < 95) || 
             (int)ch == 96 || (int)ch > 122 
            )
        {
            throw std::string("Invalid data format");
            return;
        }
    }
}
