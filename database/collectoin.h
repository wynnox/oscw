#ifndef COLLECTION_H
#define COLLECTION_H

#include "container.h"
#include "data.h"
#include <string>
#include <iostream>

class collection final : public container<std::string, data>
{
public:
    explicit collection(size_t t) : container(t) {}

    void add_data(const std::string& key, const data& value)
    {
        add_item(key, value);
    }

    const data& get_data(const std::string& key) const
    {
        return get_item(key);
    }

    void remove_data(const std::string& key)
    {
        remove_item(key);
    }

    friend std::ostream& operator<<(std::ostream& os, const collection& coll)
    {
        os << static_cast<const container<std::string, data>&>(coll);
        return os;
    }
};

#endif // COLLECTION_H
