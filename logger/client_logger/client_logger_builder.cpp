#include "client_logger_builder.h"
#include "client_logger.h"


client_logger_builder::client_logger_builder(): builder_struct_of_log("%d %t %s %m") {}

client_logger_builder::client_logger_builder(std::string struct_of_log)
        : builder_struct_of_log(std::move(struct_of_log)) {}

client_logger_builder::client_logger_builder(
    client_logger_builder const &other) = default;

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder const &other) = default;

client_logger_builder::client_logger_builder(
    client_logger_builder &&other) noexcept = default;

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder &&other) noexcept = default;

client_logger_builder::~client_logger_builder() noexcept = default;

logger_builder *client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    auto it = builder_streams.find(stream_file_path);

    if(it != builder_streams.end())
    {
        // Получение ссылки на std::set по ключу
        std::set<logger::severity>& severity_set = builder_streams[stream_file_path].second;

        // Добавление элемента в std::set
        severity_set.insert(severity);
    }
    else
    {
        auto file_stream = new std::ofstream(stream_file_path);
        if (!file_stream->is_open())
        {
            delete file_stream;
            throw std::runtime_error("Failed to open file stream " + stream_file_path);
        }

        std::set<logger::severity> severity_set;

        severity_set.insert(severity);

        builder_streams[stream_file_path] = std::make_pair(file_stream, severity_set);
    }

    return this;
}

logger_builder *client_logger_builder::add_console_stream(
    logger::severity severity)
{
    auto it = builder_streams.find("console");

    if(it != builder_streams.end())
    {
        // Получение ссылки на std::set по ключу
        std::set<logger::severity>& severity_set = builder_streams["console"].second;

        // Добавление элемента в std::set
        severity_set.insert(severity);
    }
    else
    {
        std::set<logger::severity> severity_set;

        severity_set.insert(severity);

        builder_streams["console"] = std::make_pair(nullptr, severity_set);
    }
    return this;
}

/**
 * @brief
 * @param configuration_file_path, configuration_path path to output settings and their severity
 */
logger_builder* client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{

    std::ifstream config_file(configuration_file_path);

    if (!config_file.is_open())
    {
        throw std::runtime_error("Failed to open configuration file");
    }

    nlohmann::json config;
    config_file >> config;

    for (auto& output : config[configuration_path])
    {
        std::string type = output["type"];
        if (type == "file")
        {
            std::string path = output["path"];
            logger::severity _severity;
            for (auto& severity : output["severity"])
            {
                _severity = string_to_severity(severity);
                add_file_stream(path, _severity);
            }
        }
        else if (type == "console")
        {
            std::vector<std::string> severity_list = output["severity"];
            logger::severity _severity;
            for (auto& severity : output["severity"])
            {
                _severity = string_to_severity(severity);
                add_console_stream(_severity);
            }

        }
        else
        {
            throw std::invalid_argument("Unknown stream type: " + type);
        }
    }

    return this;
}

logger_builder* client_logger_builder::transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path_output_settings,
        std::string const &configuration_path_log_settings)
{

    std::ifstream config_file(configuration_file_path);

    if (!config_file.is_open())
    {
        throw std::runtime_error("Failed to open configuration file");
    }

    nlohmann::json config;
    config_file >> config;

    for (auto& output : config[configuration_path_output_settings])
    {
        std::string type = output["type"];
        if (type == "file")
        {
            std::string path = output["path"];
            logger::severity _severity;
            for (auto& severity : output["severity"])
            {
                _severity = string_to_severity(severity);
                add_file_stream(path, _severity);
            }
        }
        else if (type == "console")
        {
            std::vector<std::string> severity_list = output["severity"];
            logger::severity _severity;
            for (auto& severity : output["severity"])
            {
                _severity = string_to_severity(severity);
                add_console_stream(_severity);
            }

        }
        else
        {
            throw std::invalid_argument("Unknown stream type: " + type);
        }
    }

    std::string log_format = config[configuration_path_log_settings];
    builder_struct_of_log = std::move(log_format);

    return this;
}

logger_builder *client_logger_builder::clear()
{
    //я хз нахуя это надо
    builder_streams.clear();
    return this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(builder_struct_of_log, builder_streams);
}