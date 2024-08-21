    #include <not_implemented.h>

#include "server_logger_builder.h"

server_logger_builder::server_logger_builder() = default;

server_logger_builder::server_logger_builder(
    server_logger_builder const &other) = default;

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder const &other) = default;

server_logger_builder::server_logger_builder(
    server_logger_builder &&other) noexcept = default;

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder &&other) noexcept = default;

server_logger_builder::~server_logger_builder() noexcept = default;

logger_builder *server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    throw not_implemented("logger_builder *server_logger_builder::add_file_stream(std::string const &stream_file_path, logger::severity severity)", "your code should be here...");
}

logger_builder *server_logger_builder::add_console_stream(
    logger::severity severity)
{
    throw not_implemented("logger_builder *server_logger_builder::add_console_stream(logger::severity severity)", "your code should be here...");
}

logger_builder* server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{
    throw not_implemented("logger_builder* server_logger_builder::transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path)", "your code should be here...");
}

logger_builder *server_logger_builder::clear()
{
    throw not_implemented("logger_builder *server_logger_builder::clear()", "your code should be here...");
}

logger *server_logger_builder::build() const
{
    return new server_logger();
}