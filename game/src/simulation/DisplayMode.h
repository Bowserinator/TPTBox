#ifndef SIMULATION_DISPLAYMODE_H_
#define SIMULATION_DISPLAYMODE_H_

#include <string>
#include <functional>
#include <array>

// Also change in part.fs
enum class DisplayMode {
    DISPLAY_MODE_VELOCITY = 0,
    DISPLAY_MODE_PRESSURE, // TODO: disable
    DISPLAY_MODE_PERSISTENT,
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
    DisplayModeData{ "Velocity Display", false },
    DisplayModeData{ "Pressure Display", false },
    DisplayModeData{ "Persistent Display", false },
    DisplayModeData{ "Heat Display", true },
    DisplayModeData{ "Fancy Display", false },
    DisplayModeData{ "Nothing Display", false },
    DisplayModeData{ "Heat Gradient Display", true }
};

#endif // SIMULATION_DISPLAYMODE_H_
