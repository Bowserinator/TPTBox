#ifndef MENU_CATEGORIES_H
#define MENU_CATEGORIES_H

#include "../interface/icons.h"

enum class MenuCategory {
    ELECTRONICS,
    POWERED,
    SENSORS,
    FORCE,
    EXPLOSIVE,
    GASES,
    LIQUIDS,
    POWDERS,
    SOLIDS,
    RADIOACTIVE,
    SPECIAL,
    LIFE,
    TOOLS,
    HIDDEN,
    LAST // Do not use, used for end marker
};

struct CategoryData {
    guiIconName icon;
    const char * name;
};

inline CategoryData CATEGORY_DATA[] = {
    { ICON_MENU_ELECTRONICS, "Electronics" },
    { ICON_MENU_POWERED,     "Powered" },
    { ICON_MENU_SENSOR,      "Sensors"},
    { ICON_MENU_FORCE,       "Force" },
    { ICON_MENU_EXPLOSIVE,   "Explosives" },
    { ICON_MENU_GASES,       "Gases" },
    { ICON_MENU_LIQUIDS,     "Liquids" },
    { ICON_MENU_POWDERS,     "Powders" },
    { ICON_MENU_SOLIDS,      "Solids" },
    { ICON_MENU_RADIOACTIVE, "Radioactive" },
    { ICON_MENU_SPECIAL,     "Special" },
    { ICON_MENU_LIFE,        "Game of Life" },
    { ICON_MENU_TOOLS,       "Tools" }
};

#endif