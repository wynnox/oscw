#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <iostream>

class database
{
public:

    virtual ~database() = default;

    virtual void add_pool(const std::string& pool_name) = 0;
    virtual void rm_pool(const std::string& pool_name) = 0;

    virtual void add_scheme(const std::string& pool_name, const std::string& scheme_name) = 0;
    virtual void rm_scheme(const std::string& pool_name, const std::string& scheme_name) = 0;

    virtual void add_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) = 0;
    virtual void rm_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) = 0;

    virtual void add_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id,
                  data _data) const = 0;
    virtual void rm_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id) const = 0;
    virtual const data& find_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id) const = 0;
    virtual std::vector<data> find_data_in_range(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id_lower_bound,
                  const std::string& id_upper_bound) const = 0;

    virtual void print_tree() const = 0;
};

#endif // DATABASE_H
