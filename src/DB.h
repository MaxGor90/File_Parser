#ifndef H_DB
#define H_DB

#include <pqxx/pqxx>
#include <string>

class DB
{
private:
    pqxx::connection* con;
    std::string connStr;
public:
    pqxx::work* work;
    pqxx::connection* getCon() const { return con; }
    DB(const std::string& connect) : connStr {connect}
    {
        con = new pqxx::connection(connStr);
        work = new pqxx::work(*con);
    }

    ~DB()
    {
        work->commit();
        delete work;
        delete con;
    }
};


#endif /* H_DB */
