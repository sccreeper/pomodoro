#include "utils.h"
#include <format>

std::string utils::formatMs(int time) {

    time = time / 1000;

    int minutes = time / 60;
    int seconds = time % 60;

    return std::format("{:0>2}:{:0>2}", minutes, seconds);

}