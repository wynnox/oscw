#include "crow.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <sstream>

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s)
{
    size_t new_length = size * nmemb;
    try {
        s->append(static_cast<char*>(contents), new_length);
    } catch (std::bad_alloc& e) {
        return 0;
    }
    return new_length;
}

class EntryPointServer
{
    std::vector<std::string> storage_servers;
    std::mutex servers_mutex;

public:
    EntryPointServer()
    {
        // Добавьте список доступных StorageServers
        storage_servers.push_back("http://localhost:8081");
        // Вы можете динамически добавлять/удалять серверы
    }

    // size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s)
    // {
    //     size_t new_length = size * nmemb;
    //     try {
    //         s->append(static_cast<char*>(contents), new_length);
    //     } catch (std::bad_alloc& e) {
    //         return 0;
    //     }
    //     return new_length;
    // }


    void run()
    {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/command").methods("POST"_method)([this](const crow::request& req)
        {
            std::string command = req.body;
            return handle_command(command);
        });


        app.port(8080).multithreaded().run();
    }

private:
crow::response handle_command(const std::string& command)
    {
        std::istringstream iss(command);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token)
        {
            tokens.push_back(token);
        }

        if (tokens.empty())
        {
            return crow::response(400, "Empty command");
        }

        std::string method, url;
        nlohmann::json command_json;
        size_t server_index = hash_command(tokens[1]) % storage_servers.size();
        int response_code = 200;

        if (tokens[0] == "ADD_POOL")
        {
            method = "POST";
            url = storage_servers[server_index] + "/pool";
            command_json["pool"] = tokens[1];
            response_code = 201;
        }
        else if (tokens[0] == "RM_POOL")
        {
            method = "DELETE";
            url = storage_servers[server_index] + "/pool/" + tokens[1];
            response_code = 204;
        }
        else if (tokens[0] == "ADD_SCHEME")
        {
            method = "POST";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme";
            command_json["scheme"] = tokens[2];
            response_code = 201;
        }
        else if (tokens[0] == "RM_SCHEME")
        {
            method = "DELETE";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme/" + tokens[2];
            response_code = 204;
        }
        else if (tokens[0] == "ADD_COLLECTION")
        {
            method = "POST";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme/" + tokens[2] + "/collection";
            command_json["collection"] = tokens[3];
            response_code = 201;
        }
        else if (tokens[0] == "RM_COLLECTION")
        {
            method = "DELETE";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme/" + tokens[2]
                + "/collection/" + tokens[3];
            response_code = 204;
        }
        else if (tokens[0] == "ADD_DATA")
        {
            method = "POST";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme/" + tokens[2]
                        + "/collection/" + tokens[3];
            command_json["id"] = tokens[4];

            json data = json::array();
            for (size_t i = 5; i < tokens.size(); ++i)
            {
                data.push_back(tokens[i]);
            }

            command_json["data"] = data;
            response_code = 201;
        }
        else if (tokens[0] == "RM_DATA")
        {
            method = "DELETE";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme/" + tokens[2]
                + "/collection/" + tokens[3] + "/data/" + tokens[4];
            response_code = 204;
        }
        else if (tokens[0] == "UPDATE_DATA")
        {
            method = "PUT";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme/" + tokens[2]
                        + "/collection/" + tokens[3] + "/data/" + tokens[4];

            json data = json::array();
            for (size_t i = 5; i < tokens.size(); ++i)
            {
                data.push_back(tokens[i]);
            }

            command_json["data"] = data;
            response_code = 202;
        }
        else if (tokens[0] == "FIND_DATA")
        {
            method = "GET";
            url = storage_servers[server_index] + "/pool/" + tokens[1] + "/scheme/" + tokens[2]
                + "/collection/" + tokens[3] + "/data/" + tokens[4];
            response_code = 200;
        }
        else
        {
            return crow::response(400, "Invalid command");
        }

        std::string json_command = command_json.dump();
        std::string storage_response = send_request_to_storage(json_command, url, method);

        return crow::response(response_code, storage_response);
    }

    std::string send_request_to_storage(const std::string& json_command, const std::string& storage_url, const std::string& method)
    {
        CURL* curl = curl_easy_init();
        std::string response_data;

        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, storage_url.c_str());

            struct curl_slist* headers = NULL;
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
            else if (method == "PUT")
            {
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_command.c_str());
            }
            else if (method == "GET")
            {
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
            }

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                std::cerr << "Failed to send command to storage server: " << curl_easy_strerror(res) << std::endl;
            }
            else
            {
                std::cout << "Received from storage server: " << response_data << std::endl;
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        else
        {
            std::cerr << "Failed to initialize cURL for storage server" << std::endl;
        }

        return response_data;
    }


    size_t hash_command(const std::string& key)
    {
        std::hash<std::string> hasher;
        return hasher(key);
    }
};

int main() {
    EntryPointServer server;
    server.run();

    return 0;
}
