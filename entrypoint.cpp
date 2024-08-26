#include "crow.h"
#include <unordered_map>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

using json = nlohmann::json;

class EntryPointServer
{
    std::unordered_map<std::string, std::string> storage_servers;
    std::mutex servers_mutex;

public:
    EntryPointServer()
    {
        storage_servers["UserPool"] = "http://localhost:8081";
    }

    void run()
    {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/pools").methods("POST"_method)([this](const crow::request& req)
        {
            auto command = json::parse(req.body);
            std::string pool = command["pool"];
            return handle_add_pool(pool);
        });

        CROW_ROUTE(app, "/pools/<string>").methods("DELETE"_method)([this](const std::string& pool) {
            return handle_rm_pool(pool);
        });

        app.port(8080).multithreaded().run();
    }

private:
    void send_request_to_storage(const std::string& json_command, const std::string& storage_url, const std::string& method) {
        CURL* curl = curl_easy_init();

        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, storage_url.c_str());

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            if (method == "POST")
            {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_command.c_str());
            }
            else if (method == "DELETE")
            {
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_command.c_str());
            }

            CURLcode res = curl_easy_perform(curl);

            if(res != CURLE_OK)
            {
                std::cerr << "Failed to send command to storage server: " << curl_easy_strerror(res) << std::endl;
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        else
        {
            std::cerr << "Failed to initialize cURL for storage server" << std::endl;
        }
    }

    crow::response handle_add_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(servers_mutex);
        storage_servers[pool] = "http://localhost:8081";

        // Создаем JSON команду для передачи на StorageServer
        nlohmann::json command_json;
        command_json["pool"] = pool;
        std::string json_command = command_json.dump();

        // Отправляем запрос на StorageServer
        send_request_to_storage(json_command, "http://localhost:8081/pools", "POST");

        return crow::response(200, "Pool " + pool + " added");
    }

    crow::response handle_rm_pool(const std::string& pool)
    {
        std::lock_guard<std::mutex> lock(servers_mutex);
        storage_servers.erase(pool);

        // Отправляем запрос на StorageServer для удаления пула
        send_request_to_storage("", "http://localhost:8081/pools/" + pool, "DELETE");

        return crow::response(200, "Pool " + pool + " removed");
    }
};

int main() {
    EntryPointServer server;
    server.run();

    return 0;
}
