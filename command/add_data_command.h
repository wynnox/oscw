#ifndef ADD_DATA_COMMAND_H
#define ADD_DATA_COMMAND_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class add_data_command final : public command
{
private:
    database* db;
    std::string pool, scheme, collection, id;
    data data_obj;

public:
    add_data_command(database* db, const std::string& pool, const std::string& scheme,
                     const std::string& collection, const std::string& id,
                     const data& _data)
        : db(db), pool(pool), scheme(scheme), collection(collection), id(id), data_obj(_data) {}

    void execute() override {
        db->add_data(pool, scheme, collection, id, data_obj);
    }
};

#endif // ADD_DATA_COMMAND_H
