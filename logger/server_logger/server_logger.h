#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include <logger.h>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>
#include "server_logger_builder.h"

class server_logger final:
    public logger
{

public:

    server_logger(
        server_logger const &other);

    server_logger &operator=(
        server_logger const &other);

    server_logger(
        server_logger &&other) noexcept;

    server_logger &operator=(
        server_logger &&other) noexcept;

    ~server_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

private:

    struct Message
    {
        std::string massage;
        logger::severity severity;
    };

    mutable std::queue<Message> _message_queue;
    mutable std::mutex _mutex;
    mutable std::condition_variable _cv;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H