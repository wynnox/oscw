#ifndef ADD_COLLECTION_H
#define ADD_COLLECTION_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class add_collection final: public command {
private:
    database* db;
    std::string pool, scheme, collection;

public:
    add_collection(database* db, const std::string& pool, const std::string& scheme,
                   const std::string& collection)
        : db(db), pool(pool), scheme(scheme), collection(collection){}

    void execute() override
    {
        db->add_collection(pool, scheme, collection);
    }
};

#endif // ADD_COLLECTION_H
