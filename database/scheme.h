#ifndef SCHEME_H
#define SCHEME_H

#include "container.h"
#include "collectoin.h"
#include <string>
#include <iostream>

class scheme final : public container<std::string, collection>
{
public:
    explicit scheme(size_t t) : container(t) {}

    virtual ~scheme() {}

    void add_collection(const std::string& collection_name, const collection& coll)
    {
        add_item(collection_name, coll);
    }

    const collection& get_collection(const std::string& collection_name) const
    {
        return get_item(collection_name);
    }

    void remove_collection(const std::string& collection_name)
    {
        remove_item(collection_name);
    }

    friend std::ostream& operator<<(std::ostream& os, const scheme& sch)
    {
        os << static_cast<const container<std::string, collection>&>(sch);
        return os;
    }

    nlohmann::json serialize_to_json() const
    {
        return container::serialize_to_json();
    }

    nlohmann::json to_json() const
    {
        return serialize_to_json();
    }
};

#endif // SCHEME_H
