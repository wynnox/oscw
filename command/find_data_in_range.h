#ifndef FIND_DATA_IN_RANGE_H
#define FIND_DATA_IN_RANGE_H

#include "command.h"
#include "database.h"
#include <string>

class find_data_in_range final: public command {
private:
    database* db;
    std::string pool, scheme, collection;
    std::string id_lower_bound, id_upper_bound;
    std::vector<data> result;

public:
    find_data_in_range(database* db, const std::string& pool, const std::string& scheme,
                   const std::string& collection,
                   const std::string& id_lower_bound, const std::string& id_upper_bound)
        : db(db), pool(pool), scheme(scheme), collection(collection),
            id_lower_bound(id_lower_bound), id_upper_bound(id_upper_bound){}

    void execute() override
    {
        result = db->find_data_in_range(pool, scheme, collection, id_lower_bound, id_upper_bound);
    }

    std::vector<data> get_result() const
    {
        return result;
    }
};

#endif // FIND_DATA_IN_RANGE_H
