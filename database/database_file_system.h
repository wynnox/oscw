#ifndef FILE_SYSTEM_DATABASE_H
#define FILE_SYSTEM_DATABASE_H

#include "database.h"
#include <filesystem>
#include "data.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


class file_system_database final: public database
{
private:
    // static inline const std::string _database = "../DATABASE";
    std::string _database;
    logger* _logger;
    mutable data cached_data;

    std::string build_path(const std::string& pool_name = "",
                           const std::string& scheme_name = "",
                           const std::string& collection_name = "",
                           const std::string& id = "") const
    {
        std::filesystem::path path = _database;
        if (!pool_name.empty()) path /= pool_name;
        if (!scheme_name.empty()) path /= scheme_name;
        if (!collection_name.empty()) path /= collection_name;
        if (!id.empty()) path /= id + ".json";
        return path.string();
    }

    bool path_exists(const std::string& pool_name,
                     const std::string& scheme_name = "",
                     const std::string& collection_name = "",
                     const std::string& id = "") const
    {
        return std::filesystem::exists(build_path(pool_name, scheme_name, collection_name, id));
    }

public:
    explicit file_system_database(std::string database_path, logger* logger) : _database("../" + database_path), _logger(logger)
    {
        if (!std::filesystem::exists(_database))
        {
            std::filesystem::create_directory(_database);
        }
        _logger->trace("CREATE B_TREE DATABASE");

    }

    ~file_system_database() override = default;

public:
    void add_pool(const std::string& pool_name) override
    {
        if (path_exists(pool_name))
        {
            std::cerr << "Пул уже существует" << std::endl;
            return;
        }

        std::filesystem::create_directory(build_path(pool_name));

        _logger->trace("ADD POOL: " + pool_name);
    }

    void rm_pool(const std::string& pool_name) override
    {
        if (!path_exists(pool_name))
        {
            std::cerr << "Пул не найден" << std::endl;
            return;
        }

        std::filesystem::remove_all(build_path(pool_name));

        _logger->trace("REMOVE POOL: " + pool_name);
    }

public:
    void add_scheme(const std::string& pool_name,
                    const std::string& scheme_name) override
    {
        if (!path_exists(pool_name) || path_exists(pool_name, scheme_name))
        {
            std::cerr << "Ошибка создания схемы" << std::endl;
            return;
        }

        std::filesystem::create_directory(build_path(pool_name, scheme_name));
        _logger->trace("ADD SCHEME: " + pool_name + "/" + scheme_name);
    }

    void rm_scheme(const std::string& pool_name,
                    const std::string& scheme_name) override
    {
        if (!path_exists(pool_name, scheme_name))
        {
            std::cerr << "Схема не найдена" << std::endl;
            return;
        }

        std::filesystem::remove_all(build_path(pool_name, scheme_name));

        _logger->trace("REMOVE SCHEME: " + pool_name + "/" + scheme_name);
    }

public:
    void add_collection(const std::string& pool_name,
                        const std::string& scheme_name,
                        const std::string& collection_name) override
    {
        if (!path_exists(pool_name) ||
            !path_exists(pool_name, scheme_name) ||
            path_exists(pool_name, scheme_name, collection_name))
        {
            std::cerr << "Ошибка создания схемы" << std::endl;
            return;
        }

        std::filesystem::create_directory(build_path(pool_name, scheme_name, collection_name));
        _logger->trace("ADD COLLECTION: " + pool_name + "/" + scheme_name + "/" + collection_name);
    }

    void rm_collection(const std::string& pool_name,
                        const std::string& scheme_name,
                        const std::string& collection_name) override
    {
        if (!path_exists(pool_name, scheme_name, collection_name))
        {
            std::cerr << "Коллекция не найдена" << std::endl;
            return;
        }

        std::filesystem::remove_all(build_path(pool_name, scheme_name, collection_name));

        _logger->trace("REMOVE COLLECTION: " + pool_name + "/" + scheme_name + "/" + collection_name);
    }

public:
    void add_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id,
                  data _data) const override
    {
        if (!path_exists(pool_name) ||
            !path_exists(pool_name, scheme_name) ||
            !path_exists(pool_name, scheme_name, collection_name))
        {
            std::cerr << "Не найдена директория для данных" << std::endl;
            return;
        }

        std::string path = build_path(pool_name, scheme_name, collection_name, id);
        std::ofstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Ошибка при открытии файла для записи" << std::endl;
            return;
        }


        nlohmann::json json_data = _data.to_json();
        file << json_data.dump();
        file.close();

        _logger->trace("ADD DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
    }

    void update_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id,
                  data _data) const override
    {
        if (!path_exists(pool_name, scheme_name, collection_name, id))
        {
            _logger->error("UPDATE DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
            throw std::runtime_error("Данные не найдены для обновления: " + id);
        }

        std::string path = build_path(pool_name, scheme_name, collection_name, id);

        std::ofstream file(path);
        if (!file.is_open())
        {
            _logger->error("Ошибка при открытии файла для обновления данных с ID: " + id);
            throw std::runtime_error("Не удалось открыть файл для обновления данных с ID: " + id);
        }

        nlohmann::json json_data = _data.to_json();
        file << json_data.dump();

        file.close();
        _logger->trace("UPDATE DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
    }

    void rm_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id) const override
    {
        if (!path_exists(pool_name, scheme_name, collection_name, id))
        {
            std::cerr << "Данные не найдены" << std::endl;
            return;
        }

        std::filesystem::remove(build_path(pool_name, scheme_name, collection_name, id));

        _logger->trace("REMOVE DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
    }

    data& find_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id) const override
    {
        if (!path_exists(pool_name, scheme_name, collection_name, id))
        {
            std::cerr << "Данные не найдены" << std::endl;
            throw std::runtime_error("Данные не найдены");
        }

        std::string path = build_path(pool_name, scheme_name, collection_name, id);
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Ошибка при открытии файла для чтения" << std::endl;
            throw std::runtime_error("Не удалось открыть файл");
        }

        nlohmann::json json_data;
        file >> json_data;
        file.close();

        cached_data = data(json_data);
        _logger->trace("FIND DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
        return cached_data;
    }

    std::vector<data> find_data_in_range(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id_lower_bound,
                  const std::string& id_upper_bound) const override
    {
        std::vector<data> result;

        for (const auto& entry : std::filesystem::directory_iterator(build_path(pool_name, scheme_name, collection_name)))
        {
            std::string filename = entry.path().filename().string();

            std::string id = filename.substr(0, filename.find_last_of('.'));

            if (id >= id_lower_bound && id <= id_upper_bound)
            {
                result.push_back(find_data(pool_name, scheme_name, collection_name, id));
            }
        }
        _logger->trace("FIND DATA IN RANGE: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id_lower_bound + " " + id_upper_bound);
        return result;
    }

    nlohmann::json serialize_tree() const override
    {
        nlohmann::json tree_json;
        std::filesystem::path path = _database;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (std::filesystem::is_regular_file(entry))
            {
                std::string relative_path = std::filesystem::relative(entry.path(), path).string();
                tree_json[relative_path] = "file";
            }
            else if (std::filesystem::is_directory(entry))
            {
                std::string relative_path = std::filesystem::relative(entry.path(), path).string();
                tree_json[relative_path] = "directory";
            }
        }

        return tree_json;
    }
};

#endif //FILE_SYSTEM_DATABASE_H
