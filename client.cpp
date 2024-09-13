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


void send_request(const std::string& command, const std::string& server_url)
{
    CURL* curl = curl_easy_init();
    std::string response_string;

    if (curl)
    {
        // std::string full_url = server_url + "/command";

        std::string full_url;

        if (command.find("REMOVE_STORAGE") == 0)
        {
            std::string port = command.substr(15);
            full_url = server_url + "/remove_storage/" + port;
        }
        else if (command == "ADD_STORAGE")
        {
            full_url = server_url + "/add_storage";
        }
        else
        {
            full_url = server_url + "/command";
        }

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
