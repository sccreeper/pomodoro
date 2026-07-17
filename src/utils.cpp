#include "utils.h"
#include <format>
#include <chrono>

std::string utils::formatMMSS(int ms) {

    ms = ms / 1000;
    std::chrono::hh_mm_ss hms{std::chrono::seconds{ms}};
    auto minutes = (hms.hours().count() * 60) + hms.minutes().count();

    return std::format("{:02}:{:02}", minutes, hms.seconds().count());

}

std::string utils::formatHHMMSS(int ms) {

    ms = ms / 1000;
    std::chrono::hh_mm_ss hms{std::chrono::seconds{ms}};

    return std::format("{:02}:{:02}:{:02}", hms.hours().count(), hms.minutes().count(), hms.seconds().count());
}