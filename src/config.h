#pragma once
#include <string>

namespace Config
{

    struct PomoConfig
    {
        int total_work_sessions;
        int total_breaks;
        int total_time_elapsed;
        int work_session_duration;
        int short_break_duration;
        int long_break_duration;
    };

    PomoConfig loadConfig(const std::string &path);
    void saveConfig(const std::string &path, PomoConfig config);

}