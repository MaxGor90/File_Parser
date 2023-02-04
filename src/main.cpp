#include <iostream>
#include <memory>
#include "IdGenerator.h"
#include "File.h"
#include "Tree.h"

unsigned int IdGenerator::Id {0};



int main(int argc, char* args[])
{
    if (argc < 3)
    {
        std::cerr << "Invalid argument input. Enter Input file as 1st arg, Output file as 2nd arg.\n";
        return 1;
    }
    std::string InFile {args[1]};
    std::string OutFile {args[2]};

    std::unique_ptr<File> file = std::make_unique<File>();
    
    try {
        file->Open(File::READ, InFile);
    } catch (const std::string& except)
    {
        std::cerr << except;
        return 1;
    }
    
    std::unique_ptr<Tree> tree = std::make_unique<Tree>();
    try
    {
        tree->Deserialize(file.get());
    }
    catch(const std::string& except)
    {
        std::cerr << except << std::endl;
        return 1;
    }
    
    try {
        file->Reopen(File::WRITE, OutFile);
    } catch (const std::string& except)
    {
        std::cerr << except;
        return 1;
    }
    tree->Serialize(file.get());

    std::cout << "Serialization completed successfully\n";
    return 0;
}