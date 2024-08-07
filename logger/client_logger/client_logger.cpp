#include "client_logger.h"

std::unordered_map<std::string, std::pair<std::ofstream *, size_t>> client_logger::_all_streams;

client_logger::client_logger(std::string _struct_of_log, std::map<std::string, std::pair<std::ofstream *, std::set<logger::severity>>> stream)
        : struct_of_log(std::move(_struct_of_log)), _streams(std::move(stream))
{
    for(const auto& pair: _streams)
    {
        _all_streams[pair.first].second++;
    }
}

// Реализация копирования
client_logger::client_logger(
    client_logger const &other)
{
    for (const auto &pair : other._streams)
    {
        _streams[pair.first] = {pair.second.first, pair.second.second};

        auto it = _all_streams.find(pair.first);
        if (it != _all_streams.end())
        {
            it->second.second++;
        }
        else
        {
            _all_streams[pair.first] = {pair.second.first, 1};
        }
    }
}

//Реализация присваивания
client_logger &client_logger::operator=(
    client_logger const &other)
{
    if(this != &other)
    {
        for(auto& pair: this->_streams)
        {
            auto it = _all_streams.find(pair.first);
            if(it != _all_streams.end())
            {
                it->second.second--;

                if(it->second.second == 0)
                {
                    it->second.first->close();
                    delete it->second.first;
                    _all_streams.erase(it);
                }
            }
        }

        for (const auto &pair : other._streams)
        {
            _streams[pair.first] = {pair.second.first, pair.second.second};

            auto it = _all_streams.find(pair.first);
            if (it != _all_streams.end())
            {
                it->second.second++;
            }
            else
            {
                _all_streams[pair.first] = {pair.second.first, 1};
            }
        }
    }
    return *this;
}

//Реализация перемещения
client_logger::client_logger(
    client_logger &&other) noexcept
{
    this->_streams = std::move(other._streams);

    other._streams.clear();
}

//Реализация присваивания с перемещением
client_logger &client_logger::operator=(
    client_logger &&other) noexcept
{
    if(this != &other)
    {
        _streams = std::move(other._streams);

        other._streams.clear();

    }
    return *this;
}

client_logger::~client_logger() noexcept
{
    auto temp_streams = _streams;

    for(auto &pair : temp_streams)
    {
        auto it = _all_streams.find(pair.first);

        if(it != _all_streams.end())
        {
            it->second.second--;

            if(it->second.second == 0)
            {
                if(it->second.first)
                {
                    it->second.first->close();

                    delete it->second.first;
                }

                _all_streams.erase(it);
            }
        }

        if(pair.second.first)
        {
            delete pair.second.first;
        }

        _streams.erase(pair.first);
    }
}

logger const *client_logger::log(
        const std::string &text,
        logger::severity severity) const noexcept
{
    auto time = std::time(nullptr);

    for(auto& pair: _streams)
    {
        if (pair.second.first && pair.second.first->is_open())
        {
            std::ostringstream formattedLog;

            for(auto& c: struct_of_log)
            {
                switch (c)
                {
                    case 'd':
                        formattedLog << std::put_time(std::localtime(&time), "%d.%m.%Y");
                        break;
                    case 't':
                        formattedLog << std::put_time(std::localtime(&time), "%H:%M:%S");
                        break;
                    case 's':
                        formattedLog << severity_to_string(severity);
                        break;
                    case 'm':
                        formattedLog << text;
                        break;
                    case '%':
                        break;
                    default:
                        formattedLog << c;
                }
            }

            *pair.second.first << formattedLog.str() << std::endl;
        }
    }

    return this;
}


void client_logger::close_streams()
{
    for (auto& pair : _streams)
    {
        if (pair.second.first)
        {
            pair.second.first->close();
            delete pair.second.first;
        }
    }
}