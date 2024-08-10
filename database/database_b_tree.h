#ifndef B_TREE_DATABASE_H
#define B_TREE_DATABASE_H

#include "b_tree.h"
#include "pool.h"
#include "database.h"
#include <string>
#include <iostream>

class b_tree_database: public database
{
private:
    b_tree<std::string, pool>* _database;
    size_t _t;

public:
    explicit b_tree_database(size_t t) : _t(t)
    {
        _database = new b_tree<std::string, pool>(_t);
    }

    ~b_tree_database() override
    {
        delete _database;
    }

    b_tree_database(const b_tree_database& other) : _t(other._t)
    {
        _database = new b_tree<std::string, pool>(*other._database);
    }

    b_tree_database& operator=(const b_tree_database& other)
    {
        if (this == &other) return *this;

        delete _database;

        _t = other._t;
        _database = new b_tree<std::string, pool>(*other._database);
        return *this;
    }

    b_tree_database(b_tree_database&& other) noexcept : _database(other._database), _t(other._t)
    {
        other._database = nullptr;
    }

    b_tree_database& operator=(b_tree_database&& other) noexcept
    {
        if (this == &other) return *this;

        delete _database;

        _database = other._database;
        _t = other._t;
        other._database = nullptr;
        return *this;
    }

public:

    void add_pool(const std::string& pool_name) override
    {
        _database->insert(pool_name, pool(_t));
        std::cout << "add pool" << std::endl;
    }

    [[nodiscard]] pool& find_pool(const std::string& pool_name) const
    {
        return const_cast<pool&>(_database->obtain(pool_name));
    }

public:

    void add_scheme(const std::string& pool_name, const std::string& scheme_name) override
    {
        find_pool(pool_name).add_scheme(scheme_name, scheme(_t));
        std::cout << "add scheme" << std::endl;
    }

    [[nodiscard]] scheme& find_scheme(const std::string& pool_name, const std::string& scheme_name) const
    {
        return const_cast<scheme&>(find_pool(pool_name).get_scheme(scheme_name));
    }

public:

    void add_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) override
    {
        find_scheme(pool_name, scheme_name).add_collection(collection_name, collection(_t));
        std::cout << "add coll" << std::endl;
    }

    [[nodiscard]] collection& find_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) const
    {
        return const_cast<collection&>(find_scheme(pool_name, scheme_name).get_collection(collection_name));
    }

public:

    void add_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id,
                  data _data) const
    {
        find_collection(pool_name, scheme_name, collection_name).add_data(id, _data);
        std::cout << "add data" << std::endl;
    }
};

#endif //B_TREE_DATABASE_H