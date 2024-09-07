#ifndef CONTAINER_H
#define CONTAINER_H

#include "b_tree.h"
#include <iostream>
#include "compasion.h"
#include <nlohmann/json.hpp>


template <typename Key, typename Value>
class container
{
private:
    b_tree<Key, Value>* _container;
    size_t _t;

protected:
    explicit container(size_t t) : _t(t), _container(new b_tree<Key, Value>(t, comparison::stdstring_comparer())) {}

    ~container()
    {
        delete _container;
    }

    container(const container& other) : _t(other._t), _container(new b_tree<Key, Value>(*other._container)) {}

    container& operator=(const container& other)
    {
        if (this == &other) return *this;

        delete _container;

        _t = other._t;
        _container = new b_tree<Key, Value>(*other._container);
        return *this;
    }

    container(container&& other) noexcept : _container(other._container), _t(other._t)
    {
        other._container = nullptr;
    }

    container& operator=(container&& other) noexcept
    {
        if (this == &other) return *this;

        delete _container;

        _container = other._container;
        _t = other._t;
        other._container = nullptr;
        return *this;
    }

    void add_item(const Key& key, const Value& value)
    {
        _container->insert(key, value, insertion_strategy::throw_an_exception);
    }

    void update_item(const Key& key, const Value& value)
    {
        _container->insert(key, value, insertion_strategy::update_value);
    }

    const Value& get_item(const Key& key) const
    {
        return _container->obtain(key);
    }

    //TODO
    std::vector<typename associative_container<Key, Value>::key_value_pair> get_item_in_range(const Key& lower_bound, const Key& upper_bound) const
    {
        return _container->obtain_between(lower_bound, upper_bound, 1, 1);
    }

    void remove_item(const Key& key)
    {
        _container->dispose(key);
    }

    friend std::ostream& operator<<(std::ostream& os, const container& container)
    {
        for (auto it = container._container->begin_infix(); it != container._container->end_infix(); ++it) {
            auto [depth, index, key, value] = *it;
            os << key << ": " << value << "\n";
        }
        return os;
    }

    nlohmann::json serialize_to_json() const
    {
        nlohmann::json json_container;

        for (auto it = _container->begin_infix(); it != _container->end_infix(); ++it) {
            auto [depth, index, key, value] = *it;
            json_container[key] = value.to_json();
        }

        return json_container;
    }

public:
    auto begin_infix() const {
        return _container->begin_infix();
    }

    auto end_infix() const {
        return _container->end_infix();
    }
};



#endif //CONTAINER_H