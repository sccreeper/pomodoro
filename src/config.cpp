#include "config.h"
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

Config::PomoConfig Config::loadConfig(const std::string &path)
{
    std::ifstream config_file{path};
    if (!config_file)
        throw std::runtime_error("Failed to open config file at " + path);
    json config_data;
    config_file >> config_data;

    Config::PomoConfig result{};

    result.total_work_sessions = config_data["total_work_sessions"];
    result.total_breaks = config_data["total_breaks"];
    result.total_time_elapsed = config_data["total_time_elapsed"];
    result.work_session_duration = config_data["work_session_duration"];
    result.short_break_duration = config_data["short_break_duration"];
    result.long_break_duration = config_data["long_break_duration"];

    return result;
}

void Config::saveConfig(const std::string &path, Config::PomoConfig config)
{
    json j;

    j["total_work_sessions"] = config.total_work_sessions;
    j["total_breaks"] = config.total_breaks;
    j["total_time_elapsed"] = config.total_time_elapsed;
    j["work_session_duration"] = config.work_session_duration;
    j["short_break_duration"] = config.short_break_duration;
    j["long_break_duration"] = config.long_break_duration;

    std::ofstream config_file;
    config_file.exceptions(std::ios::failbit | std::ios::badbit);

    try {
        config_file.open(path);
    } catch (const std::ios_base::failure&) {
        const int err = errno;
        throw std::runtime_error(
            "Failed to save config file at " + path + ": " +
            std::system_category().message(err)
        );
    }

    config_file << std::setw(4) << j << std::endl;
}