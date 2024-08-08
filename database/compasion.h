#ifndef COMPASION_N
#define COMPASION_N

#include "associative_container.h"
#include <string>

namespace comparison
{

    class int_comparer final
    {

    public:

        int operator()(
            int const &left,
            int const &right) const noexcept
        {
            return left - right;
        }

    };

    class stdstring_comparer final
    {

    public:

        int operator()(
            std::string const &first,
            std::string const &second) const noexcept
        {
            if (first == second)
            {
                return 0;
            }

            if (first > second)
            {
                return 1;
            }

            return -1;
        }

    };

    class ac_kvp_int_stdstring_comparer final
    {

    public:

        int operator()(
            typename associative_container<int, std::string>::key_value_pair const &first,
            typename associative_container<int, std::string>::key_value_pair const &second)
        {
            auto keys_comparison_result = int_comparer()(first.key, second.key);
            if (keys_comparison_result != 0) return keys_comparison_result;
            return stdstring_comparer()(first.value, second.value);
        }

    };

}

#endif //COMPASION_N