#ifndef UPDATED_DATA_H
#define UPDATED_DATA_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class update_data final: public command {
private:
    database* db;
    std::string pool, scheme, collection, id;
    // std::vector<std::string> data_args;
    data _data;

public:
    update_data(database* db, const std::string& pool, const std::string& scheme,
                   const std::string& collection, const std::string& id,
                   const data& _dataa)
        : db(db), pool(pool), scheme(scheme), collection(collection), id(id), _data(_dataa) {}

    void execute() override
    {
        db->update_data(pool, scheme, collection, id, _data);
    }
};

#endif // UPDATED_DATAD_H
