#ifndef SCHEME_H
#define SCHEME_H

#include "search_tree.h"
#include <string>
#include "collectoin.h"
#include "b_tree.h"

//представляем как поток
class scheme final
{
private:
    search_tree<std::string, collection>* _scheme;
    size_t _t;

public:
    scheme(size_t t): _t(t) {
        _scheme = new b_tree<std::string, collection>(_t);
    }

    ~scheme() {
        delete _scheme;
    }

    void add_collection(const std::string& collection_name, const collection& coll) {
        _scheme->insert(collection_name, coll);
    }

    collection get_collection(const std::string& collection_name) {
        return _scheme->obtain(collection_name);
    }

    void remove_collection(const std::string& collection_name) {
        _scheme->dispose(collection_name);
    }

    friend std::ostream& operator<<(std::ostream& os, const scheme& sch) {
        for (auto it = sch._scheme->begin_infix(); it != sch._scheme->end_infix(); ++it) {
            auto [depth, index, key, value] = *it;
            os << "Collection " << key << ":\n" << value << "\n";
        }
        return os;
    }
};

#endif //SCHEME_H