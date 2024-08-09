#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <iostream>

class database
{
public:

    virtual ~database(){}

    virtual void add_pool(const std::string& pool_name) = 0;

    virtual void add_scheme(const std::string& pool_name, const std::string& scheme_name) = 0;

    virtual void add_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) = 0;

    virtual void add_data(const std::string& pool_name,
                  const std::string& scheme_name,
                  const std::string& collection_name,
                  const std::string& id,
                  data _data) const = 0;

    // virtual collection& find_collection(const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name) const = 0;
};

#endif // DATABASE_H
