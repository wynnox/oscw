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

class StorageServer
{
    std::mutex store_mutex;
    database* _db;

public:
    StorageServer(database* db): _db(db) {}

    void run()
    {
        crow::SimpleApp app;

        // POOL

        CROW_ROUTE(app, "/pool").methods("POST"_method)([this](const crow::request& req)
        {
            auto command = json::parse(req.body);
            std::string pool = command["pool"];
            return handle_add_pool(pool);
        });

        CROW_ROUTE(app, "/pool/<string>").methods("DELETE"_method)([this](const std::string& pool)
        {
            return handle_rm_pool(pool);
        });

        // SCHEMES
        CROW_ROUTE(app, "/pool/<string>/scheme").methods("POST"_method)([this](const crow::request& req, const std::string& pool)
        {
            auto command = json::parse(req.body);
            std::string scheme = command["scheme"];
            return handle_add_scheme(pool, scheme);
        });

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>").methods("DELETE"_method)([this](const std::string& pool,
                                                                                            const std::string& scheme)
        {
            return handle_rm_scheme(pool, scheme);
        });

        // COLLECTION

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>/collection").methods("POST"_method)([this](const crow::request& req, const std::string& pool,
                                                                                            const std::string& scheme)
        {
            auto command = json::parse(req.body);
            std::string collection = command["collection"];
            return handle_add_collection(pool, scheme, collection);
        });

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>/collection/<string>").methods("DELETE"_method)([this](const std::string& pool,
                                                                                            const std::string& scheme,
                                                                                            const std::string& collection)
        {
            return handle_rm_collection(pool, scheme, collection);
        });

        // DATA

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>/collection/<string>").methods("POST"_method)([this](const crow::request& req, const std::string& pool,
                                                                                            const std::string& scheme,
                                                                                            const std::string& collection)
        {
            auto command = json::parse(req.body);
            std::string id = command["id"];
            json data = command["data"];
            return handle_add_data(pool, scheme, collection, id, data);
        });

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>/collection/<string>/data/<string>").methods("DELETE"_method)([this](const std::string& pool,
                                                                                                                   const std::string& scheme,
                                                                                                                   const std::string& collection,
                                                                                                                   const std::string& id)
        {
            return handle_rm_data(pool, scheme, collection, id);
        });

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>/collection/<string>/data/<string>").methods("PUT"_method)([this](const crow::request& req, const std::string& pool,
                                                                                                             const std::string& scheme,
                                                                                                             const std::string& collection,
                                                                                                             const std::string& id)
        {
            auto command = json::parse(req.body);
            json data = command["data"];
            return handle_update_data(pool, scheme, collection, id, data);
        });

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>/collection/<string>/data/<string>").methods("GET"_method)([this](const std::string& pool,
                                                                                                                     const std::string& scheme,
                                                                                                                     const std::string& collection,
                                                                                                                     const std::string& id)
        {
            return handle_get_data(pool, scheme, collection, id);
        });


        app.port(8081).multithreaded().run();
    }

private:
    crow::response handle_add_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        add_pool cmd(_db, pool);
        cmd.execute();

        return crow::response(201, "Pool added successfully on storage server");
    }

    crow::response handle_rm_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_pool cmd(_db, pool);
        cmd.execute();

        return crow::response(204, "Pool removed successfully on storage server");
    }

private:
    crow::response handle_add_scheme(const std::string& pool, const std::string& scheme)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        add_scheme cmd(_db, pool, scheme);
        cmd.execute();

        return crow::response(201, "Scheme added successfully on storage server");
    }

    crow::response handle_rm_scheme(const std::string& pool, const std::string& scheme)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_scheme cmd(_db, pool, scheme);
        cmd.execute();

        return crow::response(204, "Scheme removed successfully on storage server");
    }

private:
    crow::response handle_add_collection(const std::string& pool, const std::string& scheme,
                                        const std::string& collection)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        add_collection cmd(_db, pool, scheme, collection);
        cmd.execute();

        return crow::response(201, "Collection added successfully on storage server");
    }

    crow::response handle_rm_collection(const std::string& pool, const std::string& scheme,
                                        const std::string& collection)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_collection cmd(_db, pool, scheme, collection);
        cmd.execute();

        return crow::response(204, "Collection removed successfully on storage server");
    }

private:
    crow::response handle_add_data(const std::string& pool, const std::string& scheme,
                                        const std::string& collection,
                                        const std::string& id,
                                        const nlohmann::json& j)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        add_data_command cmd(_db, pool, scheme, collection, id, data(j));
        cmd.execute();

        return crow::response(201, "Data added successfully on storage server");
    }

    crow::response handle_rm_data(const std::string& pool, const std::string& scheme,
                                    const std::string& collection,
                                    const std::string& id)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_data cmd(_db, pool, scheme, collection, id);
        cmd.execute();

        return crow::response(204, "Data removed successfully on storage server");
    }

    crow::response handle_update_data(const std::string& pool, const std::string& scheme,
                                  const std::string& collection, const std::string& id,
                                  const nlohmann::json& j)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        update_data cmd(_db, pool, scheme, collection, id, data(j));
        cmd.execute();

        return crow::response(204, "Data updated successfully on storage server");
    }

    crow::response handle_get_data(const std::string& pool, const std::string& scheme,
                                  const std::string& collection, const std::string& id)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        find_data_command cmd(_db, pool, scheme, collection, id);
        cmd.execute();

        data result = cmd.get_result();

        if (result.first_name.empty() && result.last_name.empty())
        {
            return crow::response(404, "Data not found");
        }

        nlohmann::json json_response = result.to_json();
        std::cout << "Returning data: " << json_response.dump() << std::endl;
        return crow::response(200, json_response.dump());
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
