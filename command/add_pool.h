#ifndef ADD_POOL_H
#define ADD_POOL_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class add_pool final: public command {
private:
    database* db;
    std::string pool;

public:
    add_pool(database* db, const std::string& pool)
        : db(db), pool(pool) {}

    void execute() override
    {
        db->add_pool(pool);
    }
};

#endif // ADD_POOL_H
