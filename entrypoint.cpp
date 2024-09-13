#include "crow.h"
#include <unordered_map>
#include <unordered_set>
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

enum class mode
{
    in_memory_cache,
    file_system
};

class EntryPointServer
{
    std::unordered_map<std::string, std::string> pool_to_server;
    std::vector<std::string> storage_servers;
    std::mutex servers_mutex;
    int next_port = 8080;
    enum::mode _mode;

public:
    EntryPointServer(enum::mode modee): _mode(modee) {}

    // Эта функция содержит маршруты для команд пользователей
    // /command - это маршрут для команд пользователя, связанные с данными
    // /add_storage - маршрут для инамическое добавление нового сервера
    // /remove_storage - маршрут для инамическое удаления сервера
    // сервер запускается на порту 8080
    void run();

private:
    //команда, в которую направляются запросы с командами типа удаление пулов, поисков и тд
    crow::response handle_command(const std::string& command);

    //функиця, которая возвращает URL сервера хранения данных с наименьшей нагрузкой, на который мы потом будем отпарвлять данные
    std::string get_least_loaded_server_url();

    //Функция отправляет запрос на сервер хранения данных
    std::string send_request_to_storage(const std::string& json_command, const std::string& storage_url, const std::string& method);


private:

    void print_all_data_from_servers()
    {
        std::cout << "все данные " << std::endl;

        for (const auto& server_url : storage_servers)
        {
            std::string all_data_url = server_url + "/all_data";
            std::string all_data_json = send_request_to_storage("", all_data_url, "GET");
            std::cout << "Данные с сервера " << server_url << ": " << all_data_json << std::endl;
        }
    }

    // Функция, которая поднимает сервер на новом порту. Для этого используется системный вызов system, который запускает
    // исполняемый файл сервера хранения с параметрами порта и режима работы (in_memory_cache или file_system).
    void start_storage_server(int port);

    //Функция останавливает сервер хранения данных по указанному URL. Для этого отправляется POST-запрос на сервер по адресу /shutdown.
    // После отправки запроса функция ждет завершения работы сервера
    void stop_storage_server(const std::string& server_url);

    // Функция добавляет новый сервер хранения данных. Она блокирует доступ к списку серверов (с помощью std::lock_guard),
    // увеличивает номер порта для нового сервера, запускает сервер через функцию start_storage_server и добавляет его URL в список активных серверов.
    // Возвращает ответ с кодом 201 (успешное создание сервера).
    crow::response add_storage_server(int port);

    // Функция удаляет сервер хранения данных с наименьшей загрузкой.
    // Перед удалением она получает все данные с этого сервера, чтобы потом импортировать их на другой сервер.
    // Затем сервер останавливается, и данные передаются на сервер с минимальной загрузкой
    crow::response remove_storage_server(int port1, int port2);

private:
    // Функция получает текущую нагрузку на сервер по его URL.
    // Для этого она отправляет GET-запрос на сервер по адресу /load и возвращает полученное значение нагрузки.
    int get_server_load(const std::string& server_url);

    // Функция находит сервер с наименьшей нагрузкой, перебирая все серверы и запрашивая их текущую нагрузку.
    // Возвращает индекс сервера с минимальной нагрузкой.
    size_t get_least_loaded_server();
};

void EntryPointServer::run()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/command").methods("POST"_method)([this](const crow::request& req)
    {
        std::string command = req.body;
        return handle_command(command);
    });

    CROW_ROUTE(app, "/add_storage/<int>").methods("POST"_method)([this](int port)
    {
        print_all_data_from_servers();
        return add_storage_server(port);
    });

    CROW_ROUTE(app, "/remove_storage/<int>/<int>").methods("POST"_method)([this](int port1, int port2)
    {
        print_all_data_from_servers();
        return remove_storage_server(port1,port2);
    });


    app.port(8080).run();
}

crow::response EntryPointServer::handle_command(const std::string& command)
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

    const std::unordered_map<std::string, size_t> command_argument_count = {
        {"ADD_POOL", 1},
        {"RM_POOL", 1},
        {"ADD_SCHEME", 2},
        {"RM_SCHEME", 2},
        {"ADD_COLLECTION", 3},
        {"RM_COLLECTION", 3},
        {"ADD_DATA", 5},
        {"RM_DATA", 4},
        {"UPDATE_DATA", 5},
        {"FIND_DATA", 4},
        {"FIND_DATA_IN_RANGE", 5}
    };

    if (command_argument_count.find(tokens[0]) == command_argument_count.end())
    {
        return crow::response(400, "Invalid command: " + tokens[0]);
    }

    size_t expected_arguments = command_argument_count.at(tokens[0]);
    if (tokens.size() - 1 < expected_arguments)
    {
        return crow::response(400, "Invalid number of arguments for " + tokens[0]);
    }

    std::string method, url;
    nlohmann::json command_json;
    int response_code = 200;

    std::string pool = tokens[1];
    std::string server_url;

    if (tokens[0] == "ADD_POOL")
    {
        if (pool_to_server.find(pool) != pool_to_server.end())
        {
            return crow::response(400, "Pool already exists: " + pool);
        }

        method = "POST";
        server_url = get_least_loaded_server_url();
        url = server_url + "/pool";
        command_json["pool"] = pool;
        pool_to_server[pool] = server_url;
        response_code = 201;
    }
    else
    {
        if (pool_to_server.find(pool) == pool_to_server.end())
        {
            return crow::response(400, "Pool not found: " + pool);
        }

        server_url = pool_to_server[pool];

        if (tokens[0] == "RM_POOL")
        {
            method = "DELETE";
            url = server_url + "/pool/" + pool;
            pool_to_server.erase(pool);
            response_code = 204;
        }
        else if (tokens[0] == "ADD_SCHEME")
        {
            method = "POST";
            url = server_url + "/pool/" + pool + "/scheme";
            command_json["scheme"] = tokens[2];
            response_code = 201;
        }
        else if (tokens[0] == "RM_SCHEME")
        {
            method = "DELETE";
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2];
            response_code = 204;
        }
        else if (tokens[0] == "ADD_COLLECTION")
        {
            method = "POST";
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2] + "/collection";
            command_json["collection"] = tokens[3];
            response_code = 201;
        }
        else if (tokens[0] == "RM_COLLECTION")
        {
            method = "DELETE";
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2]
                + "/collection/" + tokens[3];
            response_code = 204;
        }
        else if (tokens[0] == "ADD_DATA")
        {
            method = "POST";
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2]
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
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2]
                + "/collection/" + tokens[3] + "/data/" + tokens[4];
            response_code = 204;
        }
        else if (tokens[0] == "UPDATE_DATA")
        {
            method = "PUT";
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2]
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
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2]
                + "/collection/" + tokens[3] + "/data/" + tokens[4];
            response_code = 200;
        }
        else if (tokens[0] == "FIND_DATA_IN_RANGE")
        {
            method = "GET";
            url = server_url + "/pool/" + pool + "/scheme/" + tokens[2]
                + "/collection/" + tokens[3] + "/lower/" + tokens[4] + "/upper/" + tokens[5];
            response_code = 200;
        }
    }

    std::string json_command = command_json.dump();
    std::string storage_response = send_request_to_storage(json_command, url, method);

    return crow::response(response_code, storage_response);
}

std::string EntryPointServer::get_least_loaded_server_url()
{
    size_t server_index = get_least_loaded_server();
    return storage_servers[server_index];
}

std::string EntryPointServer::send_request_to_storage(const std::string& json_command, const std::string& storage_url, const std::string& method)
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

void EntryPointServer::start_storage_server(int port)
{
    std::string mode_str;

    if (_mode == mode::in_memory_cache)
    {
        mode_str = "in_memory_cache";
    }
    else if (_mode == mode::file_system)
    {
        mode_str = "file_system";
    }

    std::string command = "/home/nncl/oscw/build/storage_program " + std::to_string(port) + " " + mode_str + " &";
    system(command.c_str());

    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void EntryPointServer::stop_storage_server(const std::string& server_url)
{
    std::string shutdown_url = server_url + "/shutdown";
    send_request_to_storage("", shutdown_url, "POST");

    std::this_thread::sleep_for(std::chrono::seconds(5));
}

//добавляем сюда по мин нагрузке
crow::response EntryPointServer::add_storage_server(int port)
{
    std::lock_guard<std::mutex> lock(servers_mutex);

    if (port == 8080)
    {
        return crow::response(404, "This port cannot be used");
    }

    std::string server_url = "http://127.0.0.1:" + std::to_string(port);

    auto it_remove = std::find(storage_servers.begin(), storage_servers.end(), server_url);
    if (it_remove != storage_servers.end())
    {
        return crow::response(404, "The server is already in use");
    }

    start_storage_server(port);

    storage_servers.push_back(server_url);
    std::cout << "Storage server added on port: " << port << std::endl;

    return crow::response(201, "Storage server added successfully");
}

crow::response EntryPointServer::remove_storage_server(int port1, int port2)
{
    std::lock_guard<std::mutex> lock(servers_mutex);

    if (storage_servers.size() <= 1)
    {
        return crow::response(400, "Cannot remove the last remaining server");
    }

    if (port1 == 8080 || port2 == 8080)
    {
        return crow::response(404, "This port cannot be used");
    }

    std::string server_to_remove = "http://127.0.0.1:" + std::to_string(port1);

    auto it_remove = std::find(storage_servers.begin(), storage_servers.end(), server_to_remove);
    if (it_remove == storage_servers.end())
    {
        return crow::response(404, "Server not found");
    }

    std::string least_loaded_server = "http://127.0.0.1:" + std::to_string(port2);
    it_remove = std::find(storage_servers.begin(), storage_servers.end(), least_loaded_server);
    if (it_remove == storage_servers.end())
    {
        return crow::response(404, "Server not found");
    }

    std::string all_data_url = server_to_remove + "/all_data";
    std::string all_data_json = send_request_to_storage("", all_data_url, "GET");

    if (all_data_json.empty())
    {
        return crow::response(500, "Error retrieving data from server");
    }

    std::cout << "Data from the server to be removed: " << all_data_json << std::endl;

    storage_servers.erase(it_remove);
    stop_storage_server(server_to_remove);

    std::string add_data_url = least_loaded_server + "/import_data";
    std::string result = send_request_to_storage(all_data_json, add_data_url, "POST");

    if (result != "Data imported successfully")
    {
        return crow::response(500, "Error importing data to the least loaded server");
    }

    std::cout << "Result of importing data to the least loaded server: " << result << std::endl;

    try
    {
        json imported_data = json::parse(all_data_json);

        if (!imported_data.is_object())
        {
            return crow::response(400, "Invalid data format during pool update");
        }

        for (auto& pool_item : imported_data.items())
        {
            std::string pool_name = pool_item.key();
            pool_to_server[pool_name] = least_loaded_server;
            std::cout << "Pool " << pool_name << " перенесен на сервер " << least_loaded_server << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка при обновлении данных о пулах: " << e.what() << std::endl;
        return crow::response(500, "Error updating pool data");
    }

    return crow::response(201, "Removing server is done");
}

int EntryPointServer::get_server_load(const std::string& server_url)
{
    CURL* curl = curl_easy_init();
    std::string response_string;
    int load = -1;

    if (curl)
    {
        std::string load_url = server_url + "/load";
        curl_easy_setopt(curl, CURLOPT_URL, load_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            load = std::stoi(response_string);
        }
        else
        {
            std::cerr << "Failed to get load from server: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return load;
}

size_t EntryPointServer::get_least_loaded_server()
{
    size_t least_loaded_server_index = 0;
    int min_load = INT_MAX;

    for (size_t i = 0; i < storage_servers.size(); ++i)
    {
        int load = get_server_load(storage_servers[i]);
        if (load < min_load)
        {
            min_load = load;
            least_loaded_server_index = i;
        }
    }

    return least_loaded_server_index;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <mode: in_memory_cache | file_system>" << std::endl;
        return 1;
    }

    std::string mode_str = argv[1];
    enum::mode selected_mode;

    if (mode_str == "in_memory_cache")
    {
        selected_mode = mode::in_memory_cache;
    }
    else if (mode_str == "file_system")
    {
        selected_mode = mode::file_system;
    }
    else
    {
        std::cerr << "Invalid mode specified. Use 'in_memory_cache' or 'file_system'." << std::endl;
        return 1;
    }

    EntryPointServer server(selected_mode);
    server.run();

    return 0;
}