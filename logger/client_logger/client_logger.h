#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H

#include <logger.h>
#include "client_logger_builder.h"
#include <map>
#include <unordered_map>
#include <set>
#include <iostream>
#include <fstream>

class client_logger final:
    public logger
{

private:

    std::map<std::string, std::pair<std::ofstream *, std::set<logger::severity>>> _streams;

    static std::unordered_map<std::string, std::pair<std::ofstream *, size_t>> _all_streams;

    std::string struct_of_log;

public:

    client_logger(std::string _struct_of_log, std::map<std::string, std::pair<std::ofstream *, std::set<logger::severity>>> stream);

    client_logger(
        client_logger const &other);

    client_logger &operator=(
        client_logger const &other);

    client_logger(
        client_logger &&other) noexcept;

    client_logger &operator=(
        client_logger &&other) noexcept;

    ~client_logger() noexcept final;

    void close_streams();

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H