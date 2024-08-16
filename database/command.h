#ifndef COMMAND_H
#define COMMAND_H
#include <iostream>
#include <string>
#include <sstream>

#include "database.h"

class command
{
private:

    /*
    ● добавление новой записи по ключу; done
    ● чтение записи по её ключу;
    ● чтение набора записей с ключами из диапазона [𝑚𝑖𝑛𝑏𝑜𝑢𝑛𝑑... 𝑚𝑎𝑥𝑏𝑜𝑢𝑛𝑑];
    ● обновление данных для записи по ключу;
    ● удаление существующей записи по ключу.

    ● добавление/удаление пулов данных;
    ● добавление/удаление схем данных для заданного пула данных;
    ● добавление/удаление коллекций данных для заданной схемы данных заданного пула
     */

public:
    command() = default;

    void execute_command(database* _db, std::string _command)
    {
        std::istringstream iss(_command);
        std::string word;

        iss >> word;

        std::string pool, scheme, collection;
        std::string id, n, sn;

        std::string extra;

        if (word == "ADD_DATA")
        {
            if(iss >> pool && iss >> scheme && iss >> collection && iss >> id && iss >> n && iss >> sn)
            {
                // TODO парсинг бы data
                if(!(iss >> extra))
                    _db->add_data(pool, scheme, collection, id, {n, sn});
            }
            else
            {
                return;
            }
        }
        //TODO
        else if (word == "FIND_DATA")
        {
            if(iss >> pool && iss >> scheme && iss >> collection && iss >> id)
            {
                // TODO парсинг бы data
                if(!(iss >> extra))
                    std::cout << _db->find_data(pool, scheme, collection, id);
            }
            else
            {

                return;
            }
        }
        //TODO
        else if (word == "FIND_DATA_IN_RANGE")
        {
            std::string id_lower_bound, id_upper_bound;
            if(iss >> pool && iss >> scheme && iss >> collection && iss >> id_lower_bound >> id_upper_bound)
            {
                // TODO парсинг бы data
                if(!(iss >> extra))
                {
                    std::vector<data> result = _db->find_data_in_range(pool, scheme, collection,
                        id_lower_bound, id_upper_bound);
                    for (const auto& item : result)
                    {
                        std::cout << item;
                    }
                }
            }
            else
            {

                return;
            }
        }
        //TODO
        else if (word == "UPDATE_DATA")
        {
            if(iss >> pool && iss >> scheme && iss >> collection && iss >> id && iss >> n && iss >> sn)
            {
                // TODO парсинг бы data
                if(!(iss >> extra))
                    _db->update_data(pool, scheme, collection, id, {n, sn});
            }
            else
            {
                return;
            }
        }
        //TODO
        else if (word == "RM_DATA")
        {
            if(iss >> pool && iss >> scheme && iss >> collection && iss >> id)
            {
                // TODO парсинг бы data
                if(!(iss >> extra))
                    _db->rm_data(pool, scheme, collection, id);
            }
            else
            {
                std::cout << "lox" << std::endl;
                return;
            }
        }
        else if (word == "ADD_POOL")
        {
            if(iss >> pool)
            {
                if(!(iss >> extra))
                    _db->add_pool(pool);
            }
            else
            {
                std::cout << "lox" << std::endl;
                return;
            }
        }
        //TODO
        else if (word == "RM_POOL")
        {
            if(iss >> pool)
            {
                if(!(iss >> extra))
                    _db->rm_pool(pool);
            }
            else
            {
                std::cout << "lox" << std::endl;
                return;
            }
        }
        else if (word == "ADD_SCHEME")
        {
            if(iss >> pool && iss >> scheme)
            {
                if(!(iss >> extra))
                    _db->add_scheme(pool, scheme);
            }
            else
            {
                std::cout << "lox" << std::endl;
                return;
            }
        }
        //TODO
        else if (word == "RM_SCHEME")
        {
            if(iss >> pool && iss >> scheme)
            {
                if(!(iss >> extra))
                    _db->rm_scheme(pool, scheme);
            }
            else
            {
                std::cout << "lox" << std::endl;
                return;
            }
        }
        else if (word == "ADD_COLLECTION")
        {
            if(iss >> pool && iss >> scheme && iss >> collection)
            {
                if(!(iss >> extra))
                    _db->add_collection(pool, scheme, collection);
            }
            else
            {
                std::cout << "lox" << std::endl;
                return;
            }
        }
        //TODO
        else if (word == "RM_COLLECTION")
        {
            if(iss >> pool && iss >> scheme && iss >> collection)
            {
                if(!(iss >> extra))
                    _db->rm_collection(pool, scheme, collection);
            }
            else
            {
                std::cout << "lox" << std::endl;
                return;
            }
        }
        else
        {
            _db->print_tree();

            return;
        }
    }
};

#endif //COMMAND_H