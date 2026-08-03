#pragma once
#include <string>

namespace utils {
    std::string formatMMSS(int64_t ms);
    std::string formatHHMMSS(int64_t ms);
    std::string formatDDMMYYY(int64_t ms);

    struct DateContainer {
        int day;
        int month;
        int year;
        int ts;
    };

    utils::DateContainer unixTimeToDate(int64_t ms);
    int64_t getUnixTimestamp(int64_t deltaDays = 0);
}