#include "crow.h"
#include <unordered_map>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "database.h"
#include "logger/client_logger/client_logger.h"
#include "logger/client_logger/client_logger_builder.h"
#include "database/database_b_tree.h"
#include "command/add_data_command.h"
#include "command/find_data_command.h"
#include "command/add_collection.h"
#include "command/add_pool.h"
#include "command/add_scheme.h"
#include "command/find_data_in_range.h"
#include "command/print.h"
#include "command/rm_collection.h"
#include "command/rm_data.h"
#include "command/rm_pool.h"
#include "command/rm_scheme.h"
#include "command/update_data.h"
#include "data.h"

using json = nlohmann::json;

class StorageServer {
    std::mutex store_mutex;
    database* _db;

public:
    StorageServer(database* db): _db(db) {}

    void run() {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/pools").methods("POST"_method)([this](const crow::request& req) {
            auto command = json::parse(req.body);
            std::string pool = command["pool"];
            return handle_add_pool(pool);
        });

        CROW_ROUTE(app, "/pools/<string>").methods("DELETE"_method)([this](const std::string& pool) {
            return handle_rm_pool(pool);
        });

        app.port(8081).multithreaded().run();
    }

private:
    crow::response handle_add_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        add_pool cmd(_db, pool);
        cmd.execute();

        return crow::response(200, "Pool added successfully on storage server");
    }

    crow::response handle_rm_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_pool cmd(_db, pool);
        cmd.execute();

        return crow::response(200, "Pool removed successfully on storage server");
    }
};

logger *create_logger(
    std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug) {

    logger_builder *builder = new client_logger_builder();

    if (use_console_stream) {
        builder->add_console_stream(console_stream_severity);
    }

    for (auto &output_file_stream_setup: output_file_streams_setup) {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }

    logger *built_logger = builder->build();

    delete builder;

    return built_logger;
}

int main() {
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"logs.txt", logger::severity::trace}
    });

    database* _database = new b_tree_database(3, logger);

    StorageServer server(_database);
    server.run();

    delete _database;
    delete logger;

    return 0;
}
