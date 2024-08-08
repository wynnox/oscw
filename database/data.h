#ifndef DATA_H
#define DATA_H

#include <string>

class data {
public:
    std::string first_name;
    std::string last_name;

    data() = default;
    data(const std::string& first_name, const std::string& last_name)
        : first_name(first_name), last_name(last_name) {}

    data(const data& other) = default;

    data& operator=(const data& other) = default;

    data(data&& other) noexcept = default;

    data& operator=(data&& other) noexcept = default;

    bool operator==(const data& other) const
    {
        return first_name == other.first_name && last_name == other.last_name;
    }

    bool operator!=(const data& other) const
    {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const data& d)
    {
        os << d.first_name << " " << d.last_name;
        return os;
    }
};

#endif //DATA_H