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

// std::string construct_url(const std::string& command, const std::string& server_url)
// {
//     std::string full_url;
//
//     if (command.find("REMOVE_STORAGE") == 0)
//     {
//         size_t first_space = command.find(" ");
//         size_t second_space = command.find(" ", first_space + 1);
//
//         std::string port1 = command.substr(first_space + 1, second_space - first_space - 1);
//         std::string port2 = command.substr(second_space + 1);
//
//         if (port1.empty() || port2.empty())
//         {
//             std::cerr << "Error: Invalid arguments for REMOVE_STORAGE. Ports cannot be empty." << std::endl;
//             return "lol";
//         }
//
//         full_url = server_url + "/remove_storage/" + port1 + "/" + port2;
//     }
//     else if (command.find("ADD_STORAGE") == 0)
//     {
//         size_t space = command.find(" ");
//         std::string port = command.substr(space + 1);
//
//         if(port.empty())
//         {
//             std::cerr << "Error: Invalid arguments for REMOVE_STORAGE. Ports cannot be empty." << std::endl;
//             return "lol";
//         }
//
//         full_url = server_url + "/add_storage/" + port;
//     }
//     else
//     {
//         full_url = server_url + "/command";
//     }
//
//     return full_url;
// }

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
        std::string full_url;

        if (command.find("REMOVE_STORAGE") == 0)
        {
            size_t first_space = command.find(" ");

            if (first_space == std::string::npos)
            {
                std::cerr << "Error: Invalid arguments for REMOVE_STORAGE. Two ports are required." << std::endl;
                return;
            }

            size_t second_space = command.find(" ", first_space + 1);

            if (second_space == std::string::npos)
            {
                std::cerr << "Error: Invalid arguments for REMOVE_STORAGE. Two ports are required." << std::endl;
                return;
            }

            std::string port1 = command.substr(first_space + 1, second_space - first_space - 1);
            std::string port2 = command.substr(second_space + 1);

            if (port1.empty() || port2.empty())
            {
                std::cerr << "Error: Invalid arguments for REMOVE_STORAGE. Ports cannot be empty." << std::endl;
                return;
            }

            full_url = server_url + "/remove_storage/" + port1 + "/" + port2;
        }
        else if (command.find("ADD_STORAGE") == 0)
        {
            size_t space = command.find(" ");

            if (space == std::string::npos)
            {
                std::cerr << "Error: Invalid arguments for ADD_STORAGE. Port is required." << std::endl;
                return;
            }

            std::string port = command.substr(space + 1);

            if(port.empty())
            {
                std::cerr << "Error: Invalid arguments for REMOVE_STORAGE. Ports cannot be empty." << std::endl;
                return;
            }

            full_url = server_url + "/add_storage/" + port;
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
            handle_response(curl, response_string);
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }
}

void menu()
{
    std::cout << "0. Меню\n";
    std::cout << "1. Ввод команд\n";
    std::cout << "2. Ввод файла\n";
    std::cout << "3. Выход\n";
    std::cout << "=============================\n";
    std::cout << "Введите номер команды: ";
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << "<server_url>" << std::endl;
        return 1;
    }

    std::string server_url = argv[1];

    std::cout << "Hello!" << std::endl;

    std::string choice;
    std::string command, input_file;

    while(true)
    {
        menu();
        std::cin >> choice;

        if (choice == "0")
        {
            menu();
        }
        else if (choice == "1")
        {
            std::cout << "Введите команду: ";
            std::cin.ignore();
            std::getline(std::cin, command);
            if (!command.empty())
            {
                send_request(command, server_url);
            }
        }
        else if (choice == "2")
        {
            std::cout << "Введите имя файла: ";
            std::cin.ignore();
            std::getline(std::cin, input_file);
            {
                std::ifstream infile(input_file);
                if (!infile.is_open())
                {
                    std::cerr << "Не удалось открыть файл: " << input_file << std::endl;
                    continue;
                }

                while (std::getline(infile, command))
                {
                    if (!command.empty())
                    {
                        send_request(command, server_url);
                    }
                }
            }
        }
        else if (choice == "3")
        {
            std::cout << "Выход из программы." << std::endl;
            return 0;
        }
        else
        {
            std::cout << "Неверная команда, попробуйте снова." << std::endl;
        }
    }


    return 0;
}
