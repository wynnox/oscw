#ifndef B_TREE_DATABASE_H
#define B_TREE_DATABASE_H

#include "b_tree.h"
#include "pool.h"
#include "database.h"
#include <string>
#include <iostream>
#include "container.h"

class b_tree_database: public database //, public container<std::string, pool>
{
private:
    container<std::string, pool>* _database;
    size_t _t;
    server_type _server_type;

    logger* _logger;

public:
    explicit b_tree_database(size_t t, logger* logger)
        : _t(t), _logger(logger), _server_type(server_type::in_memory_cache)
    {
        _database = new container<std::string, pool>(t);
        _logger->trace("CREATE B_TREE DATABASE");
    }

    ~b_tree_database() override
    {
        _logger->trace("DELETE B_TREE DATABASE");
        delete _database;
    }

    b_tree_database(const b_tree_database& other)
    : _t(other._t), _server_type(other._server_type), _logger(other._logger)
    {
        _database = new container<std::string, pool>(*other._database);
    }

    b_tree_database& operator=(const b_tree_database& other)
    {
        if (this == &other) return *this;

        _t = other._t;
        _server_type = other._server_type;
        _logger = other._logger;

        delete _database;
        _database = new container<std::string, pool>(*other._database);

        return *this;
    }

    b_tree_database(b_tree_database&& other) noexcept
        : _database(other._database), _t(other._t), _server_type(other._server_type), _logger(other._logger)
    {
        other._database = nullptr;
        other._logger = nullptr;
    }

    server_type get_server_type() const override
    {
        return _server_type;
    }


public:

    void add_pool(const std::string& pool_name) override
    {
        try
        {
            _logger->trace("Attempting to insert pool with key: " + pool_name);
            _database->add_item(pool_name, pool(_t));
            _logger->trace("Pool added successfully with key: " + pool_name);
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
            _database->remove_item(pool_name);
        }
        catch(const std::exception &e)
        {
            throw std::logic_error("Error removing pool: " + pool_name + ": " + e.what());
        }
    }

    [[nodiscard]] pool& find_pool(const std::string& pool_name) const
    {
        return const_cast<pool&>(_database->get_item(pool_name));
    }

public:

    void add_scheme(const std::string& pool_name, const std::string& scheme_name) override
    {
        try
        {
            _logger->trace("lol");
            find_pool(pool_name).add_item(scheme_name, scheme(_t));
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

    void update_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id,
                  data _data) const override
    {
        try
        {
            find_collection(pool_name, scheme_name, collection_name).update_data(id, _data);
            _logger->trace("UPDATE DATA: " + pool_name + "/" + scheme_name + "/" + collection_name + "/" + id);
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

    nlohmann::json serialize_tree() const override
    {
        nlohmann::json json_tree;

        for (auto it = _database->begin_infix(); it != _database->end_infix(); ++it)
        {
            auto [level, index, pool_name, pool_value] = *it;

            const container<std::string, scheme>& pool_container = pool_value;

            nlohmann::json pool_json;

            for (auto scheme_it = pool_container.begin_infix(); scheme_it != pool_container.end_infix(); ++scheme_it) {
                auto [scheme_level, scheme_index, scheme_name, scheme_value] = *scheme_it;

                const container<std::string, collection>& scheme_container = scheme_value;

                nlohmann::json scheme_json;

                for (auto collection_it = scheme_container.begin_infix(); collection_it != scheme_container.end_infix(); ++collection_it) {
                    auto [collection_level, collection_index, collection_name, collection_value] = *collection_it;

                    const container<std::string, data>& collection_container = collection_value;

                    nlohmann::json collection_json;

                    for (auto data_it = collection_container.begin_infix(); data_it != collection_container.end_infix(); ++data_it) {
                        auto [data_level, data_index, data_id, data_value] = *data_it;

                        collection_json[data_id] = data_value.to_json();
                    }

                    scheme_json[collection_name] = collection_json;
                }

                pool_json[scheme_name] = scheme_json;
            }

            json_tree[pool_name] = pool_json;
        }

        std::cout << "nlohmann::json serialize_tree() const override :" << json_tree << std::endl;
        return json_tree;
    }

};

#endif //B_TREE_DATABASE_H