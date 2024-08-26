#ifndef FIND_DATA_COMMAND_H
#define FIND_DATA_COMMAND_H

#include "command.h"
#include "database.h"
#include <string>

class find_data_command final: public command {
private:
    database* db;
    std::string pool, scheme, collection, id;
    data result;

public:
    find_data_command(database* db, const std::string& pool, const std::string& scheme,
                   const std::string& collection, const std::string& id)
        : db(db), pool(pool), scheme(scheme), collection(collection), id(id) {}

    void execute() override
    {
        result = db->find_data(pool, scheme, collection, id);
    }

    data get_result() const
    {
        return result;
    }
};

#endif // FIND_DATA_COMMAND_H
