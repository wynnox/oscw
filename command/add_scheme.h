#ifndef ADD_SCHEME_H
#define ADD_SCHEME_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class add_scheme final: public command {
private:
    database* db;
    std::string pool, scheme;

public:
    add_scheme(database* db, const std::string& pool, const std::string& scheme)
        : db(db), pool(pool), scheme(scheme) {}

    void execute() override
    {
        db->add_scheme(pool, scheme);
    }
};

#endif // ADD_SCHEME_H
