#ifndef H_FILE
#define H_FILE

#include <fstream>


class File
{
public:

    File() {}
    ~File();

    enum write_read { WRITE, READ } Mode;
    
    std::ofstream out_file;
    std::ifstream in_file;

    void Open(write_read mode, const std::string& path);
    void Reopen(write_read mode, const std::string& path);

};

#endif /* H_FILE */
