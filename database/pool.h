#ifndef POOL_H
#define POOL_H

#include "container.h"
#include "scheme.h"
#include <string>
#include <iostream>

class pool final : public container<std::string, scheme>
{
public:
    explicit pool(size_t t) : container(t) {}

    void add_scheme(const std::string& scheme_name, const scheme& sch)
    {
        add_item(scheme_name, sch);
    }

    const scheme& get_scheme(const std::string& scheme_name) const
    {
        return get_item(scheme_name);
    }

    void remove_scheme(const std::string& scheme_name)
    {
        remove_item(scheme_name);
    }

    friend std::ostream& operator<<(std::ostream& os, const pool& pl)
    {
        os << static_cast<const container<std::string, scheme>&>(pl);
        return os;
    }

    nlohmann::json serialize_to_json() const
    {
        return container::serialize_to_json();  // Use container's serialization
    }
};

#endif // POOL_H
