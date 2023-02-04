#include "File.h"
#include <iostream>



void File::Open(write_read mode, const std::string& path)
{   
    Mode = mode;
    
    switch (Mode)
    {
    case READ:
    {
        in_file.open(path, std::ios::binary);
        if (!in_file.is_open())
            throw std::string(path + " could not be opened for reading\n");
        break;

    }
    case WRITE:
    {
        out_file.open(path, std::ios::binary | std::ios::trunc);
        if (!out_file.is_open())
            throw std::string(path + " could not be opened for writing\n");
        break;
    }
    default:
        break;
    }
}

void File::Reopen(write_read mode, const std::string& path)
{
    if (in_file.is_open())
        in_file.close();
    if (out_file.is_open())
        out_file.close();
    Open(mode, path);
}


File::~File() 
{
    if (in_file.is_open())
        in_file.close();
    
    if (out_file.is_open())
        out_file.close();
}

