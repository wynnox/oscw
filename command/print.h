#ifndef PRINT_H
#define PRINT_H

#include "command.h"
#include "database.h"
#include <string>
#include <vector>

class print final: public command {
private:
    database* db;

public:
    print(database* db)  : db(db){}

    void execute() override
    {
        db->print_tree();
    }
};

#endif // RM_COLLECTION_H
