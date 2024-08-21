#ifndef RM_SCHEME_H
#define RM_SCHEME_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class rm_scheme final: public command {
private:
    database* db;
    std::string pool, scheme;

public:
    rm_scheme(database* db, const std::string& pool, const std::string& scheme)
        : db(db), pool(pool), scheme(scheme) {}

    void execute() override
    {
        db->rm_scheme(pool, scheme);
    }
};

#endif // RM_SCHEME_H
