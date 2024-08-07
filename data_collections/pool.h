#ifndef POOL_H
#define POOL_H

#include "search_tree.h"
#include <string>
#include "scheme.h"
#include "b_tree.h"

//представляем как курс
class pool final
{
private:
    search_tree<std::string, scheme>* _pool;
    size_t _t;

public:
    pool(size_t t): _t(t)
    {
        _pool = new b_tree<std::string, scheme>(_t);
    }

    ~pool()
    {
        delete _pool;
    }

    void add_scheme(const std::string& scheme_name, const scheme& sch)
    {
        _pool->insert(scheme_name, sch);
    }

    scheme get_scheme(const std::string& scheme_name)
    {
        return _pool->obtain(scheme_name);
    }

    void remove_scheme(const std::string& scheme_name)
    {
        _pool->dispose(scheme_name);
    }

    friend std::ostream& operator<<(std::ostream& os, const pool& pl)
    {
        for (auto it = pl._pool->begin_infix(); it != pl._pool->end_infix(); ++it)
        {
            auto [depth, index, key, value] = *it;
            os << "Scheme " << key << ":\n" << value << "\n";
        }
        return os;
    }

};

#endif //POOL_H