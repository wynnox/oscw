#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "command.h"
#include "add_data_command.h"
#include "find_data_command.h"

#include <memory>
#include <sstream>
#include <string>

#include "add_collection.h"
#include "add_pool.h"
#include "add_scheme.h"
#include "find_data_in_range.h"
#include "print.h"
#include "rm_collection.h"
#include "rm_data.h"
#include "rm_pool.h"
#include "rm_scheme.h"
#include "update_data.h"

class command_handler {
public:
    std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void execute_command(database* db, const std::string& _command) {
        auto args = split(_command);

        if (args.empty())
        {
            throw std::logic_error("Empty command.");
        }

        std::unique_ptr<command> cmd;

        if (args[0] == "ADD_DATA") {
            if (args.size() >= 5)
            {
                std::vector<std::string> data_args(std::make_move_iterator(args.begin() + 5),
                            std::make_move_iterator(args.end()));
                cmd = std::make_unique<add_data_command>(db, args[1], args[2], args[3], args[4], std::move(data_args));
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for ADD_DATA");
            }
        }
        else if (args[0] == "FIND_DATA")
        {
            if (args.size() == 5)
            {
                cmd = std::make_unique<find_data_command>(db, args[1], args[2], args[3], args[4]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for FIND_DATA");
            }
        }
        else if(args[0] == "FIND_DATA_IN_RANGE")
        {
            if (args.size() == 6)
            {
                cmd = std::make_unique<find_data_in_range>(db, args[1], args[2], args[3], args[4], args[5]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for FIND_DATA_IN_RANGE");
            }
        }
        else if(args[0] == "UPDATE_DATA")
        {
            if (args.size() >= 5)
            {
                std::vector<std::string> data_args(std::make_move_iterator(args.begin() + 5),
                            std::make_move_iterator(args.end()));
                cmd = std::make_unique<update_data>(db, args[1], args[2], args[3], args[4], std::move(data_args));
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for UPDATE_DATA");
            }
        }
        else if (args[0] == "RM_DATA")
        {
            if (args.size() == 5)
            {
                cmd = std::make_unique<rm_data>(db, args[1], args[2], args[3], args[4]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for RM_DATA");
            }
        }
        else if (args[0] == "ADD_POOL")
        {
            if (args.size() == 2)
            {
                cmd = std::make_unique<add_pool>(db, args[1]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for ADD_POOL");
            }
        }
        else if (args[0] == "RM_POOL")
        {
            if (args.size() == 2)
            {
                cmd = std::make_unique<rm_pool>(db, args[1]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for RM_POOL");
            }
        }
        else if (args[0] == "ADD_SCHEME")
        {
            if (args.size() == 3)
            {
                cmd = std::make_unique<add_scheme>(db, args[1], args[2]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for ADD_SCHEME");
            }
        }
        else if (args[0] == "RM_SCHEME")
        {
            if (args.size() == 3)
            {
                cmd = std::make_unique<rm_scheme>(db, args[1], args[2]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for RM_SCHEME");
            }
        }
        else if (args[0] == "ADD_COLLECTION")
        {
            if (args.size() == 4)
            {
                cmd = std::make_unique<add_collection>(db, args[1], args[2], args[3]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for ADD_COLLECTION");
            }
        }
        else if (args[0] == "RM_COLLECTION")
        {
            if (args.size() == 4)
            {
                cmd = std::make_unique<rm_collection>(db, args[1], args[2], args[3]);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for RM_COLLECTION");
            }
        }
        else if (args[0] == "PRINT")
        {
            if (args.size() == 1)
            {
                cmd = std::make_unique<print>(db);
            }
            else
            {
                throw std::logic_error("Invalid number of arguments for PRINT");
            }
        }

        if (cmd)
        {
            cmd->execute();
        }
        else
        {
            throw std::logic_error("Command not recognized or failed to create.");
        }
    }
};

#endif // COMMAND_HANDLER_H
