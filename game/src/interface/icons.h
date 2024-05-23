#ifndef TPB_ICONS_H
#define TPB_ICONS_H

#include "../../resources/generated/icons.png.h"

inline constexpr float RAYGUI_ICON_SIZE = 16; // Size of icons (squared)
inline constexpr unsigned int RAYGUI_ICON_MAX_ICONS = 128;

typedef enum {
    ICON_NONE = 0,
    ICON_UNDO_FILL,
    ICON_IMAGE_SETTINGS,
    ICON_FILE,
    ICON_SIM_SETTINGS,
    ICON_PLAYER_PAUSE,
    ICON_PLAYER_PLAY,
    ICON_MENU_ELECTRONICS,
    ICON_MENU_POWERED,
    ICON_MENU_SENSOR,
    ICON_MENU_FORCE,
    ICON_MENU_EXPLOSIVE,
    ICON_MENU_GASES,
    ICON_MENU_LIQUIDS,
    ICON_MENU_POWDERS,
    ICON_MENU_SOLIDS,
    ICON_MENU_RADIOACTIVE,
    ICON_MENU_SPECIAL,
    ICON_MENU_LIFE,
    ICON_MENU_TOOLS,
} guiIconName;

#endif