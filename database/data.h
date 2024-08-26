#ifndef DATA_H
#define DATA_H

#include <string>
#include <nlohmann/json.hpp>

class data
{
public:
    std::string first_name;
    std::string last_name;

    data() = default;
    data(const std::string& first_name, const std::string& last_name)
        : first_name(first_name), last_name(last_name) {}
    data(const std::vector<std::string>&& data_arg)
    {
        if(data_arg.size() != 2)
        {
            throw std::logic_error("Invalid number of arguments for NEW_DATA");
        }
        //TODO
        first_name = std::move(data_arg[0]);
        last_name = std::move(data_arg[1]);
    }

    friend void to_json(nlohmann::json& j, const data& d) {
        j = nlohmann::json{{"first_name", d.first_name}, {"last_name", d.last_name}};
    }

    friend void from_json(const nlohmann::json& j, data& d) {
        j.at("first_name").get_to(d.first_name);
        j.at("last_name").get_to(d.last_name);
    }

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
        os << d.first_name << " " << d.last_name << std::endl;
        return os;
    }
};

#endif //DATA_H