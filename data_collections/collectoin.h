#ifndef COLLECTOIN_H
#define COLLECTOIN_H

#include "data.h"
#include "search_tree.h"
#include "b_tree.h"

//представляем как группу
class collection final
{
private:
    search_tree<std::string, data>* _collection;
    size_t _t;

public:
    collection(size_t t): _t(t)
    {
        _collection = new b_tree<std::string, data>(_t);
    }

    ~collection()
    {
        delete _collection;
    }

    void add_data(const std::string& key, const data& value) {
        _collection->insert(key, value);
    }

    data get_data(const std::string& key)
    {
        return _collection->obtain(key);
    }

    void remove_data(const std::string& key)
    {
        _collection->dispose(key);
    }

    friend std::ostream& operator<<(std::ostream& os, const collection& coll) {
        for (auto it = coll._collection->begin_infix(); it != coll._collection->end_infix(); ++it) {
            auto [depth, index, key, value] = *it;
            os << key << ": " << value << "\n";
        }
        return os;
    }
};

#endif //COLLECTOIN_H