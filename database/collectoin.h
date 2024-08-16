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

    void update_data(const std::string& key, const data& value)
    {
        update_item(key, value);
    }

    const data& get_data(const std::string& key) const
    {
        return get_item(key);
    }

    std::vector<data> get_data_in_range(const std::string& key1, const std::string& key2) const
    {
        std::vector<data> result;
        auto items = get_item_in_range(key1, key2);

        for (const auto& item : items) {
            result.push_back(item.value);
        }

        return result;
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
