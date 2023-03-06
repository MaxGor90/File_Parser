#include <iostream>
#include <memory>
#include "IdGenerator.h"
#include "File.h"
#include "Tree.h"
#include "DB.h"

unsigned int IdGenerator::Id {0};



int main(int argc, char* args[])
{
    setlocale(LC_ALL, "Russian");

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

    try {
        DB db("host=localhost port=5432 dbname=learn user=postgres");

        db.work->exec0("DROP TABLE IF EXISTS public.shape_params;");
        db.work->exec0(
        "CREATE TABLE shape_params ("
            "id int PRIMARY KEY, "
            "parent_id int references shape_params(id), "
            "name text, "
            "value text );"
        );

        tree->WriteToDB(&db);

        std::string q{
            "select s2.id, s2.name, s1.id as Par_ID, s1.name as Par_Name  FROM shape_params as s1 "
            "RIGHT JOIN shape_params as s2 "
                "ON s2.parent_id = s1.id "
            "WHERE s1.name = 'vertices';"
        };
        pqxx::result res {db.work->exec(q)};

        for (auto row : res)
        {
            for (auto col : row)
            {
                std::cout << ' ' << col;
            }
            std::cout << std::endl;
        }

    } catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    return 0;
}