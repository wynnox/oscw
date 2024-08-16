#ifndef FILE_SYSTEM_DATABASE_H
#define FILE_SYSTEM_DATABASE_H

#include "database.h"
#include <filesystem>
#include "data.h"
#include <iostream>
#include <fstream>


class file_system_database final: public database
{
private:
    static inline const std::string _database = "DATABASE";

    std::string build_path(const std::string& pool_name = "",
                           const std::string& scheme_name = "",
                           const std::string& collection_name = "") const
    {
        std::filesystem::path path = _database;
        if (!pool_name.empty()) path /= pool_name;
        if (!scheme_name.empty()) path /= scheme_name;
        if (!collection_name.empty()) path /= collection_name;
        return path.string();
    }

    bool path_exists(const std::string& pool_name,
                     const std::string& scheme_name = "",
                     const std::string& collection_name = "") const
    {
        return std::filesystem::exists(build_path(pool_name, scheme_name, collection_name));
    }

public:
    explicit file_system_database()
    {
        if (std::filesystem::exists(_database))
        {
            std::cout << "lol тут не продуманая логика" << std::endl;
            std::cout << std::filesystem::path();
            return;
        }

        std::filesystem::create_directory(_database);
    }

    ~file_system_database() override = default;

public:
    void add_pool(const std::string& pool_name) override
    {
        if (path_exists(pool_name))
        {
            std::cout << "lol тут не продуманая логика" << std::endl;
            return;
        }

        std::filesystem::create_directory(build_path(pool_name));
    }

public:
    void add_scheme(const std::string& pool_name,
                    const std::string& scheme_name) override
    {
        if (!path_exists(pool_name) || path_exists(pool_name, scheme_name))
        {
            std::cout << "lol тут не продуманая логика" << std::endl;
            return;
        }

        std::filesystem::create_directory(build_path(pool_name, scheme_name));
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
            std::cout << "lol тут не продуманая логика" << std::endl;
            return;
        }

        std::filesystem::create_directory(build_path(pool_name, scheme_name, collection_name));
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
            std::cout << "lol тут не продуманая логика" << std::endl;
            return;
        }

        //сделать как страничную организацию памяти
    }

};

#endif //FILE_SYSTEM_DATABASE_H
