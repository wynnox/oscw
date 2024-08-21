#ifndef RM_POOL_H
#define RM_POOL_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class rm_pool final: public command {
private:
    database* db;
    std::string pool;

public:
    rm_pool(database* db, const std::string& pool)
        : db(db), pool(pool) {}

    void execute() override
    {
        db->rm_pool(pool);
    }
};

#endif // RM_POOL_H
