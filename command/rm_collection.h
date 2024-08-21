#ifndef RM_COLLECTION_H
#define RM_COLLECTION_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class rm_collection final: public command {
private:
    database* db;
    std::string pool, scheme, collection;

public:
    rm_collection(database* db, const std::string& pool, const std::string& scheme,
                   const std::string& collection)
        : db(db), pool(pool), scheme(scheme), collection(collection){}

    void execute() override
    {
        db->rm_collection(pool, scheme, collection);
    }
};

#endif // RM_COLLECTION_H
