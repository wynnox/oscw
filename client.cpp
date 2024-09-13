#include <iostream>
#include <fstream>
#include <curl/curl.h>

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

std::string construct_url(const std::string& command, const std::string& server_url)
{
    std::string full_url;

    if (command.find("REMOVE_STORAGE") == 0)
    {
        size_t first_space = command.find(" ");
        size_t second_space = command.find(" ", first_space + 1);

        std::string port1 = command.substr(first_space + 1, second_space - first_space - 1);
        std::string port2 = command.substr(second_space + 1);

        full_url = server_url + "/remove_storage/" + port1 + "/" + port2;
    }
    else if (command.find("ADD_STORAGE") == 0)
    {
        size_t space = command.find(" ");
        std::string port = command.substr(space + 1);

        full_url = server_url + "/add_storage/" + port;
    }
    else
    {
        full_url = server_url + "/command";
    }

    return full_url;
}

void handle_response(CURL* curl, const std::string& response_string)
{
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    std::cout << "HTTP Response Code: " << response_code << std::endl;
    switch (response_code)
    {
        case 200:
            std::cout << "Operation successful: " << response_string << std::endl;
            break;
        case 201:
            break;
        case 202:
            break;
        case 204:
            break;
        case 404:
            break;
        default:
            std::cerr << "Failed with HTTP code: " << response_code << std::endl;
        break;
    }
}

void send_request(const std::string& command, const std::string& server_url)
{
    CURL* curl = curl_easy_init();
    std::string response_string;

    if (curl)
    {
        std::string full_url = construct_url(command, server_url);

        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, command.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Failed to send command: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            handle_response(curl, response_string);
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }
}


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
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
            send_request(command, server_url);
        }

    }

    return 0;
}
