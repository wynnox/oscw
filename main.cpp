#include <iostream>
#include <string>
#include <fstream>

#include "database_b_tree.h"
#include "database.h"
// #include "database_file_system.h"
#include <crow.h>

#include "command/command_handler.h"
#include "logger/client_logger/client_logger.h"

logger *create_logger(
    std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug)
{

    logger_builder *builder = new client_logger_builder();

    if (use_console_stream)
    {
        builder->add_console_stream(console_stream_severity);
    }

    for (auto &output_file_stream_setup: output_file_streams_setup)
    {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }

    logger *built_logger = builder->build();

    delete builder;

    return built_logger;
}


int main()
{
    crow::SimpleApp app;

    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {"logs.txt", logger::severity::trace}
    });

    database* _database = new b_tree_database(3, logger);
    command_handler _command_handler;

    CROW_ROUTE(app, "/execute").methods("POST"_method)([&_database, &_command_handler](const crow::request &req) {
        try {
            std::string command_string = req.body;
            _command_handler.execute_command(_database, command_string);
            return crow::response(200, "Command executed successfully");
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Error executing command: ") + e.what());
        }
    });

    app.port(8080).multithreaded().run();

    delete _database;
    delete logger;

    return 0;
}

// int main()
// {
//     std::string command1 = "-in_memory_cache";
//     std::string command2 = "-file_system";
//
//     // command _command;
//     command_handler _command;
//
//     logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
//     {
//         { "logs.txt", logger::severity::trace }
//     });
//
//     try {
//         database* _database = new b_tree_database(3, logger);
//
//         std::ifstream input_file("file.txt");
//
//         if (!input_file.is_open())
//         {
//             std::cerr << "error with opening file" << std::endl;
//             return 1;
//         }
//
//         std::string command_string;
//         while (std::getline(input_file, command_string))
//         {
//             _command.execute_command(_database, command_string);
//         }
//         input_file.close();
//
//         delete _database;
//         delete logger;
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
//
//     return 0;
// }
