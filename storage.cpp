#include "crow.h"
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
#include "command/rm_collection.h"
#include "command/rm_data.h"
#include "command/rm_pool.h"
#include "command/rm_scheme.h"
#include "command/update_data.h"
#include "data.h"
#include "database_file_system.h"

using json = nlohmann::json;

class StorageServer
{
    std::mutex store_mutex;
    database* _db;

    int load_data = 0, load_collection = 0, load_scheme = 0, load_pool = 0;

public:
    StorageServer(database* db): _db(db) {}

    void run(crow::SimpleApp& app, int port)
    {
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

        CROW_ROUTE(app, "/pool/<string>/scheme/<string>/collection/<string>/lower/<string>/upper/<string>").methods("GET"_method)([this](const std::string& pool,
                                                                                                                                  const std::string& scheme,
                                                                                                                                  const std::string& collection,
                                                                                                                                  const std::string& lower,
                                                                                                                                  const std::string& upper)
        {
            return handle_get_data_in_range(pool, scheme, collection, lower, upper);
        });


        CROW_ROUTE(app, "/shutdown").methods("POST"_method)([this, &app]()
        {
            std::lock_guard<std::mutex> lock(store_mutex);

            if (_db->get_server_type() == database::server_type::file_system)
            {
                std::string db_path = dynamic_cast<file_system_database*>(_db)->get_database_path();
                std::filesystem::remove_all(db_path);
                std::cout << "Removed file system database directory: " << db_path << std::endl;
            }


            std::cout << "Shutting down storage server" << std::endl;
            app.stop();

            return crow::response(200, "Server shutting down");
        });

        CROW_ROUTE(app, "/all_data").methods("GET"_method)([this]()
        {
            std::lock_guard<std::mutex> lock(store_mutex);

            try
            {
                nlohmann::json json_response = _db->serialize_tree();
                return crow::response(200, json_response.dump());
            }
            catch (const std::exception& e)
            {
                return crow::response(500, "Error serializing data: " + std::string(e.what()));
            }
        });


        CROW_ROUTE(app, "/load").methods("GET"_method)([this]()
        {
            std::lock_guard<std::mutex> lock(store_mutex);
            int load = load_data + load_collection + load_scheme + load_pool;
            return crow::response(200, std::to_string(load));
        });


        CROW_ROUTE(app, "/import_data").methods("POST"_method)([this](const crow::request& req)
        {
            std::lock_guard<std::mutex> lock(store_mutex);

            std::cout << "Received raw request body: " << req.body << std::endl;

            try {
                auto json_data = nlohmann::json::parse(req.body);
                std::cout << "Parsed JSON: " << json_data.dump(4) << std::endl;

                if (!json_data.is_object()) {
                    std::cerr << "Error: JSON is not an object" << std::endl;
                    return crow::response(400, "Invalid JSON structure for pools");
                }

                for (const auto& pool_item : json_data.items())
                {
                    std::string pool_name = pool_item.key();
                    auto pool_value = pool_item.value();
                    std::cout << "Processing pool: " << pool_name << std::endl;

                    if (!pool_value.is_object()) {
                        std::cerr << "Error: Pool is not an object" << std::endl;
                        return crow::response(400, "Invalid JSON structure for pool " + pool_name);
                    }

                    add_pool cmd(_db, pool_name);
                    cmd.execute();
                    load_pool++;

                    for (const auto& scheme_item : pool_item.value().items())
                    {
                        std::string scheme_name = scheme_item.key();
                        auto scheme_value = scheme_item.value();
                        std::cout << "Processing scheme: " << scheme_name << " in pool: " << pool_name << std::endl;

                        if (!scheme_value.is_object()) {
                            std::cerr << "Error: Scheme is not an object" << std::endl;
                            return crow::response(400, "Invalid JSON structure: Expected a JSON object for scheme " + scheme_name);
                        }

                        add_scheme cmd1(_db, pool_name, scheme_name);
                        cmd1.execute();

                        for (const auto& collection_item : scheme_item.value().items())
                        {
                            std::string collection_name = collection_item.key();
                            auto collection_value = collection_item.value();
                            std::cout << "Processing collection: " << collection_name << " in scheme: " << scheme_name << std::endl;

                            if (!collection_value.is_object()) {
                                std::cerr << "Error: Collection is not an object" << std::endl;
                                return crow::response(400, "Invalid JSON structure: Expected a JSON object for collection " + collection_name);
                            }

                            add_collection cmd2(_db, pool_name, scheme_name, collection_name);
                            cmd2.execute();

                            for (const auto& data_item : collection_item.value().items())
                            {
                                std::string data_id = data_item.key();
                                json data_value = data_item.value();
                                std::cout << "Processing data: ID = " << data_id << " in collection: " << collection_name << std::endl;


                                try {
                                    add_data_command cmd4(_db, pool_name, scheme_name, collection_name, data_id, data(data_value));
                                    cmd4.execute();
                                } catch (const std::exception& e) {
                                    std::cerr << "Error adding data: " << e.what() << std::endl;
                                    return crow::response(500, "Error adding data: " + std::string(e.what()));
                                }
                            }
                        }
                    }
                }

                return crow::response(200, "Data imported successfully");
            }
            catch (const std::exception& e) {
                std::cerr << "Exception while importing data: " << e.what() << std::endl;
                return crow::response(500, "Error while importing data");
            }
        });

        app.port(port).run();
    }

private:
    crow::response handle_add_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        if (_db->get_server_type() == database::server_type::in_memory_cache && _db->pool_exists(pool))
        {
            return crow::response(400, "Pool already exists on this storage server: " + pool);
        }

        add_pool cmd(_db, pool);
        cmd.execute();
        load_pool++;

        return crow::response(201, "Pool added successfully on storage server");
    }

    crow::response handle_rm_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_pool cmd(_db, pool);
        cmd.execute();
        load_pool--;

        return crow::response(204, "Pool removed successfully on storage server");
    }

private:
    crow::response handle_add_scheme(const std::string& pool, const std::string& scheme)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        add_scheme cmd(_db, pool, scheme);
        cmd.execute();
        load_scheme++;

        return crow::response(201, "Scheme added successfully on storage server");
    }

    crow::response handle_rm_scheme(const std::string& pool, const std::string& scheme)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_scheme cmd(_db, pool, scheme);
        cmd.execute();
        load_scheme--;

        return crow::response(204, "Scheme removed successfully on storage server");
    }

private:
    crow::response handle_add_collection(const std::string& pool, const std::string& scheme,
                                        const std::string& collection)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        add_collection cmd(_db, pool, scheme, collection);
        cmd.execute();
        load_collection++;

        return crow::response(201, "Collection added successfully on storage server");
    }

    crow::response handle_rm_collection(const std::string& pool, const std::string& scheme,
                                        const std::string& collection)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_collection cmd(_db, pool, scheme, collection);
        cmd.execute();
        load_collection--;

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
        load_data++;

        return crow::response(201, "Data added successfully on storage server");
    }

    crow::response handle_rm_data(const std::string& pool, const std::string& scheme,
                                    const std::string& collection,
                                    const std::string& id)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        rm_data cmd(_db, pool, scheme, collection, id);
        cmd.execute();
        load_data--;

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

    crow::response handle_get_data_in_range(const std::string& pool, const std::string& scheme,
                              const std::string& collection, const std::string& lower, const std::string& upper)
    {
        std::lock_guard<std::mutex> lock(store_mutex);

        find_data_in_range cmd(_db, pool, scheme, collection, lower, upper);
        cmd.execute();

        std::vector<data> result = cmd.get_result();

        nlohmann::json json_response = nlohmann::json::array();
        for (const auto& entry : result)
        {
            json_response.push_back(entry.to_json());
        }

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

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <mode: in_memory_cache | file_system>"<< std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    std::string file = "logs" + std::to_string(port) + ".txt";

    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {file, logger::severity::trace}
    });

    database* _database;
    if(argv[2] == std::string("in_memory_cache"))
    {
        _database = new b_tree_database(3, logger);
    }
    else if(argv[2] == std::string("file_system"))
    {
        std::string db_path = "server_db_" + std::to_string(port);
        _database = new file_system_database(db_path, logger);
    }
    else
    {
        std::cout << "я больной";
        return 0;
    }

    StorageServer server(_database);

    crow::SimpleApp app;
    server.run(app, port);

    delete _database;
    delete logger;

    return 0;
}