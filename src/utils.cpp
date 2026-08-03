#include "utils.h"
#include <format>
#include <chrono>

std::string utils::formatMMSS(int64_t ms)
{
    std::chrono::hh_mm_ss hms{std::chrono::seconds{ms / 1000}};
    auto minutes = (hms.hours().count() * 60) + hms.minutes().count();

    return std::format("{:02}:{:02}", minutes, hms.seconds().count());
}

std::string utils::formatHHMMSS(int64_t ms)
{
    std::chrono::hh_mm_ss hms{std::chrono::seconds{ms / 1000}};

    return std::format("{:02}:{:02}:{:02}", hms.hours().count(), hms.minutes().count(), hms.seconds().count());
}

std::string utils::formatDDMMYYY(int64_t ms)
{
    auto tp = std::chrono::system_clock::from_time_t(ms / 1000);
    return std::format("{:%d/%m/%Y}", tp);
}

utils::DateContainer utils::unixTimeToDate(int64_t ms)
{
    using namespace std::chrono;
    auto tp = system_clock::from_time_t(ms / 1000);
    auto dp = floor<days>(tp);
    year_month_day ymd{dp};

    return utils::DateContainer{
        int(ymd.year()),
        int(unsigned(ymd.month())),
        int(unsigned(ymd.day()))};
}

int64_t utils::getUnixTimestamp(int64_t deltaDays)
{
    using namespace std::chrono;

    auto tz = current_zone();
    auto local_midnight = floor<days>(tz->to_local(system_clock::now()));
    auto adjusted_local_midnight = local_midnight + days{deltaDays};

    return duration_cast<seconds>(
               tz->to_sys(adjusted_local_midnight, choose::earliest).time_since_epoch())
        .count();
}