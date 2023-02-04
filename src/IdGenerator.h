#ifndef H_IDGENERATOR
#define H_IDGENERATOR

class IdGenerator
{
    static unsigned int Id;
public:
    static unsigned int generateId() { return ++Id; }
};



#endif /* H_IDGENERATOR */
