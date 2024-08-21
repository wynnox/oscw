#ifndef ADD_DATA_COMMAND_H
#define ADD_DATA_COMMAND_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class add_data_command final: public command {
private:
    database* db;
    std::string pool, scheme, collection, id;
    std::vector<std::string> data_args;

public:
    add_data_command(database* db, const std::string& pool, const std::string& scheme,
                   const std::string& collection, const std::string& id,
                   std::vector<std::string>&& _data_args)
        : db(db), pool(pool), scheme(scheme), collection(collection), id(id), data_args(std::move(_data_args)) {}

    void execute() override
    {
        db->add_data(pool, scheme, collection, id, data(std::move(data_args)));
    }
};

#endif // ADD_DATA_COMMAND_H
