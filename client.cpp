#include <iostream>
#include <fstream>
#include "curl/curl.h"
#include <nlohmann/json.hpp>
#include <vector>

void send_request(const std::string& json_command, const std::string& server_url, const std::string& method) {
    CURL* curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());

        if (method == "POST")
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_command.c_str());
        }
        else if (method == "PUT")
        {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_command.c_str());
        }
        else if (method == "DELETE")
        {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_command.c_str());
        }
        else if (method == "GET")
            {
            // GET не отправляет тело, так что просто устанавливаем URL
        }

        CURLcode res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
            std::cerr << "Failed to send command: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }
}

void send_command_to_entrypoint(const std::string& command, const std::string& server_url)
{
    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token)
    {
        tokens.push_back(token);
    }

    nlohmann::json command_json;
    std::string method, url;

    if (tokens[0] == "ADD_POOL")
    {
        method = "POST";
        url = server_url + "pools";
        command_json["pool"] = tokens[1];
    } else if (tokens[0] == "RM_POOL") {
        method = "DELETE";
        url = server_url + "pools/" + tokens[1];
    // } else if (tokens[0] == "ADD_SCHEME") {
    //     method = "POST";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes";
    //     command_json["scheme"] = tokens[2];
    // } else if (tokens[0] == "RM_SCHEME") {
    //     method = "DELETE";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes/" + tokens[2];
    // } else if (tokens[0] == "ADD_COLLECTION") {
    //     method = "POST";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes/" + tokens[2] + "/collections";
    //     command_json["collection"] = tokens[3];
    // } else if (tokens[0] == "RM_COLLECTION") {
    //     method = "DELETE";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes/" + tokens[2] + "/collections/" + tokens[3];
    // } else if (tokens[0] == "ADD_DATA") {
    //     method = "POST";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes/" + tokens[2] + "/collections/" + tokens[3] + "/data";
    //     command_json["id"] = tokens[4];
    //     for (size_t i = 5; i < tokens.size(); ++i) {
    //         command_json["data"].push_back(tokens[i]);
    //     }
    // } else if (tokens[0] == "UPDATE_DATA") {
    //     method = "PUT";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes/" + tokens[2] + "/collections/" + tokens[3] + "/data/" + tokens[4];
    //     for (size_t i = 5; i < tokens.size(); ++i) {
    //         command_json["data"].push_back(tokens[i]);
    //     }
    // } else if (tokens[0] == "FIND_DATA") {
    //     method = "GET";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes/" + tokens[2] + "/collections/" + tokens[3] + "/data/" + tokens[4];
    // } else if (tokens[0] == "RM_DATA") {
    //     method = "DELETE";
    //     url = server_url + "/pools/" + tokens[1] + "/schemes/" + tokens[2] + "/collections/" + tokens[3] + "/data/" + tokens[4];
    // } else if (tokens[0] == "PRINT") {
    //     method = "GET";
    //     url = server_url + "/print";
    } else {
        throw std::logic_error("Invalid command");
    }

    std::string json_command = command_json.dump();
    send_request(json_command, url, method);
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <command_file> <server_url>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string server_url = argv[2];

    std::ifstream infile(input_file);
    if (!infile.is_open())
    {
        std::cerr << "Could not open file: " << input_file << std::endl;
        return 1;
    }

    std::string command;
    while (std::getline(infile, command))
    {
        if (!command.empty())
        {
            std::cout << "Sending command: " << command << std::endl;
            send_command_to_entrypoint(command, server_url);
        }
    }

    return 0;
}
