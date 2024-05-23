#ifndef DISPLAY_MODE_H
#define DISPLAY_MODE_H

#include <string>
#include <functional>
#include <array>

enum class DisplayMode {
    DISPLAY_MODE_PRESSURE = 0,
    DISPLAY_MODE_PERSISTENT,
    DISPLAY_MODE_BLOB,
    DISPLAY_MODE_HEAT,
    DISPLAY_MODE_FANCY,
    DISPLAY_MODE_NOTHING,
    DISPLAY_MODE_HEAT_GRADIENT,
    LAST // DO NOT USE
};

struct DisplayModeData {
    std::string name;
    bool alwaysUpdate;
};

const std::array<DisplayModeData, (std::size_t)DisplayMode::LAST> displayModeProperties{
    DisplayModeData{ "Pressure Display", false },
    DisplayModeData{ "Persistent Display", false },
    DisplayModeData{ "Blob Display", false },
    DisplayModeData{ "Heat Display", true },
    DisplayModeData{ "Fancy Display", false },
    DisplayModeData{ "Nothing Display", false },
    DisplayModeData{ "Heat Gradient Display", true }
}; 

#endif