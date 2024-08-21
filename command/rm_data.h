#ifndef RM_DATA_H
#define RM_DATA_H

#include "command.h"
#include "database.h"
#include <string>

class rm_data final: public command {
private:
    database* db;
    std::string pool, scheme, collection, id;

public:
    rm_data(database* db, const std::string& pool, const std::string& scheme,
                   const std::string& collection, const std::string& id)
        : db(db), pool(pool), scheme(scheme), collection(collection), id(id) {}

    void execute() override
    {
        db->rm_data(pool, scheme, collection, id);
    }
};

#endif // RM_DATA_H
