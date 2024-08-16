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

    logger* _logger;

public:
    explicit b_tree_database(size_t t, logger* logger) : _t(t), _logger(logger)
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
        try
        {
            _logger->trace("ADD POOL: " + pool_name);
            _database->insert(pool_name, pool(_t));
        }
        catch (const std::exception &e)
        {
            throw std::logic_error("Error adding pool: " + pool_name + ": " + e.what());
        }
    }

    void rm_pool(const std::string& pool_name) override
    {
        try
        {
            _logger->trace("REMOVE POOL: " + pool_name);
            _database->dispose(pool_name);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error removing pool: " + pool_name + ": " + e.what());
        }
    }

    [[nodiscard]] pool& find_pool(const std::string& pool_name) const
    {
        return const_cast<pool&>(_database->obtain(pool_name));
    }

public:

    void add_scheme(const std::string& pool_name, const std::string& scheme_name) override
    {
        try
        {
            find_pool(pool_name).add_scheme(scheme_name, scheme(_t));
            _logger->trace("ADD SCHEME: " + pool_name + "/" + scheme_name);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error adding scheme: " + pool_name + "/" + scheme_name + ": " + e.what());
        }
    }

    void rm_scheme(const std::string& pool_name, const std::string& scheme_name) override
    {
        try
        {
            _logger->trace("REMOVE SCHEME: " + pool_name + "/" + scheme_name);
            find_pool(pool_name).remove_scheme(scheme_name);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error removing scheme: " + pool_name + "/" + scheme_name + ": " + e.what());
        }
    }

    [[nodiscard]] scheme& find_scheme(const std::string& pool_name, const std::string& scheme_name) const
    {
        return const_cast<scheme&>(find_pool(pool_name).get_scheme(scheme_name));
    }

public:

    void add_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) override
    {
        try
        {
            find_scheme(pool_name, scheme_name).add_collection(collection_name, collection(_t));
            _logger->trace("ADD COLLECTION: " + pool_name + "/" + scheme_name + "/" + collection_name);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error adding collection: " + pool_name + "/" + scheme_name + "/" + collection_name + ": " + e.what());
        }

    }

    void rm_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) override
    {
        try
        {
            _logger->trace("REMOVE COLLECTION: " + pool_name + "/" + scheme_name + "/" + collection_name);
            find_scheme(pool_name, scheme_name).remove_collection(collection_name);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error removing collection: " + pool_name + "/" + scheme_name + "/" + collection_name + ": " + e.what());
        }
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
                  data _data) const override
    {
        try
        {
            find_collection(pool_name, scheme_name, collection_name).add_data(id, _data);
            _logger->trace("ADD DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error adding data: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id + ": " + e.what());
        }

    }

    void rm_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id) const override
    {
        try
        {
            find_collection(pool_name, scheme_name, collection_name).remove_data(id);
            _logger->trace("REMOVE DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error removing data: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id + ": " + e.what());
        }
    }

    const data& find_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id) const override
    {
        try
        {
            _logger->trace("FIND DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
            return find_collection(pool_name, scheme_name, collection_name).get_data(id);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error findind data: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id + ": " + e.what());
        }
    }

    std::vector<data> find_data_in_range(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id_lower_bound,
                  const std::string& id_upper_bound)const override
    {
        try
        {
            _logger->trace("FIND DATA IN RANGE: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id_lower_bound + " " + id_upper_bound);
            return find_collection(pool_name, scheme_name, collection_name).get_data_in_range(id_lower_bound, id_upper_bound);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error findind data: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id_lower_bound + " " + id_upper_bound + ": " + e.what());
        }
    }


public:

    void print_tree() const override
    {
        if (!_database)
        {
            std::cout << "Database is not initialized." << std::endl;
            return;
        }

        auto it = _database->begin_infix();
        auto end = _database->end_infix();

        if (it == end)
        {
            std::cout << "Tree is empty." << std::endl;
            return;
        }

        for (; it != end; ++it)
        {
            auto [level, index, key, value] = *it;
            std::cout << std::string(level * 2, ' ') << "Level " << level << ", Index " << index << ": "
                      << key << " => " << value << std::endl;
        }
    }
};

#endif //B_TREE_DATABASE_H