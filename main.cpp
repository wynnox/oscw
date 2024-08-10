#include <iostream>
#include <string>

#include "database_b_tree.h"
#include "pool.h"
#include "collectoin.h"
#include "data.h"
#include "database.h"
#include "database_file_system.h"


int main()
{
    std::string command1 = "-in_memory_cache";
    std::string command2 = "-file_system";


    try {
        database* _database = new b_tree_database(3);
        _database->add_pool("data_pool");
        _database->add_scheme("data_pool", "student_scheme");
        _database->add_collection("data_pool", "student_scheme", "group_a");
        _database->add_collection("data_pool", "student_scheme", "group_b");

        _database->add_data("data_pool", "student_scheme",
            "group_a", "a", {"John", "Doe"});

        _database->add_data("data_pool", "student_scheme",
            "group_a", "b", {"Jane", "Smith"});

        _database->add_data("data_pool", "student_scheme",
            "group_b", "c", {"Emily", "Jones"});

        _database->add_data("data_pool", "student_scheme",
            "group_b", "d", {"Michael", "Brown"});

        std::cout << "lol" << std::endl;

        delete _database;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    try {
        database* _database = new file_system_database();
        _database->add_pool("data_pool");
        _database->add_scheme("data_pool", "student_scheme");
        _database->add_collection("data_pool", "student_scheme", "group_a");
        _database->add_collection("data_pool", "student_scheme", "group_b");

        _database->add_data("data_pool", "student_scheme",
            "group_a", "a", {"John", "Doe"});

        _database->add_data("data_pool", "student_scheme",
            "group_a", "b", {"Jane", "Smith"});

        _database->add_data("data_pool", "student_scheme",
            "group_b", "c", {"Emily", "Jones"});

        _database->add_data("data_pool", "student_scheme",
            "group_b", "d", {"Michael", "Brown"});

        std::cout << "lol" << std::endl;

        delete _database;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
