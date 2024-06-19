#ifndef INTERFACE_SIM_CONSOLE_COMMANDS_SET_H_
#define INTERFACE_SIM_CONSOLE_COMMANDS_SET_H_

#include <string>
#include <vector>

#include "../../../../simulation/Simulation.h"
#include "../../../../util/str_format.h"
#include "../../../../util/colored_text.h"

namespace commands {
    inline auto cmd_set = [](Simulation * sim, const std::vector<std::string> &tokens) -> std::string {
        if (tokens.size() != 4)
            return text_format::F_RED + "Error: invalid syntax for set";

        enum class PartProp { type, flag, ctype, life, x, y, z, vx, vy, vz, temp, tmp1, tmp2, dcolor, invalid };
        enum class ValueType { t_float, t_uint, t_int, t_temperature, t_part_type };
        ValueType prop_types[] = { ValueType::t_part_type, ValueType::t_uint, ValueType::t_uint, // type, flag, ctype
            ValueType::t_int, ValueType::t_uint, ValueType::t_uint, ValueType::t_uint, // life, x, y, z
            ValueType::t_float, ValueType::t_float, ValueType::t_float, // vx, vy, vz
            ValueType::t_temperature, ValueType::t_uint, ValueType::t_uint, // temp, tmp1, tmp2
            ValueType::t_uint // dcolor
        };

        PartProp prop_to_edit = PartProp::invalid;
        if (tokens[1] == "type")
            prop_to_edit = PartProp::type;
        // else if (tokens[1] == "flag") // Removed: potential to crash game / break stuff
        //     prop_to_edit = PartProp::flag;
        else if (tokens[1] == "ctype")
            prop_to_edit = PartProp::ctype;
        else if (tokens[1] == "life")
            prop_to_edit = PartProp::life;
        else if (tokens[1] == "x")
            prop_to_edit = PartProp::x;
        else if (tokens[1] == "y")
            prop_to_edit = PartProp::y;
        else if (tokens[1] == "z")
            prop_to_edit = PartProp::z;
        else if (tokens[1] == "vx")
            prop_to_edit = PartProp::vx;
        else if (tokens[1] == "vy")
            prop_to_edit = PartProp::vy;
        else if (tokens[1] == "vz")
            prop_to_edit = PartProp::vz;
        else if (tokens[1] == "temp")
            prop_to_edit = PartProp::temp;
        else if (tokens[1] == "tmp1" || tokens[1] == "tmp")
            prop_to_edit = PartProp::tmp1;
        else if (tokens[1] == "tmp2")
            prop_to_edit = PartProp::tmp2;
        else if (tokens[1] == "dcolor")
            prop_to_edit = PartProp::dcolor;

        if (prop_to_edit == PartProp::invalid)
            return text_format::F_RED + "Error: unknown property " + tokens[1];

        bool apply_to_all = tokens[2] == "all";
        part_type filter = 0;
        if (!apply_to_all) {
            auto tmp = util::parse_string_part_type(tokens[2]);
            if (tmp == std::nullopt)
                return text_format::F_RED + "Error: invalid type: " + tokens[2];
            filter = tmp.value();
        }

        std::size_t parts_updated = 0;

        // Check value to set
        part_type val_ptype = 0;
        float val_float = 0.0f;
        unsigned int val_uint = 0;
        int val_int = 0;

        switch (prop_types[(int)prop_to_edit]) {
            case ValueType::t_float: {
                auto tmp = util::parse_string_float(tokens[3]);
                if (tmp == std::nullopt)
                    return text_format::F_RED + "Error: invalid float value: " + tokens[3];
                val_float = tmp.value();
                break;
            }
            case ValueType::t_uint: {
                auto tmp = util::parse_string_integer<unsigned int>(tokens[3]);
                if (tmp == std::nullopt)
                    return text_format::F_RED + "Error: invalid uint value: " + tokens[3];
                val_uint = tmp.value();
                break;
            }
            case ValueType::t_int: {
                auto tmp = util::parse_string_integer<int>(tokens[3]);
                if (tmp == std::nullopt)
                    return text_format::F_RED + "Error: invalid int value: " + tokens[3];
                val_int = tmp.value();
                break;
            }
            case ValueType::t_temperature: {
                auto tmp = util::temp_string_to_kelvin(tokens[3]);
                if (tmp == std::nullopt)
                    return text_format::F_RED + "Error: invalid temperature value: " + tokens[3];
                val_float = tmp.value();
                break;
            }
            case ValueType::t_part_type: {
                auto tmp = util::parse_string_part_type(tokens[3]);
                if (tmp == std::nullopt)
                    return text_format::F_RED + "Error: invalid type: " + tokens[3];
                val_ptype = tmp.value();
                break;
            }
        }

        // Bounds check for location
        if (prop_to_edit == PartProp::x && (val_uint == 0 || val_uint >= XRES - 1))
            return text_format::F_RED + "Error: x must be between 1 and " + std::to_string(XRES - 2) + " inclusive";
        if (prop_to_edit == PartProp::y && (val_uint == 0 || val_uint >= YRES - 1))
            return text_format::F_RED + "Error: x must be between 1 and " + std::to_string(YRES - 2) + " inclusive";
        if (prop_to_edit == PartProp::z && (val_uint == 0 || val_uint >= ZRES - 1))
            return text_format::F_RED + "Error: x must be between 1 and " + std::to_string(ZRES - 2) + " inclusive";

        for (part_id i = 1; i <= sim->maxId; i++) {
            if (!sim->parts[i].type) continue;
            if (!apply_to_all && sim->parts[i].type != filter) continue;
            parts_updated++;

            switch (prop_to_edit) {
                case PartProp::type:
                    sim->part_change_type(i, val_ptype);
                    break;
                case PartProp::flag:
                    sim->parts[i].flag = static_cast<decltype(sim->parts[i].flag)>(val_uint);
                    break;
                case PartProp::ctype:
                    sim->parts[i].ctype = static_cast<decltype(sim->parts[i].ctype)>(val_uint);
                    break;
                case PartProp::life:
                    sim->parts[i].life = static_cast<decltype(sim->parts[i].life)>(val_int);
                    break;

                case PartProp::x:
                    sim->try_move(i, val_uint, sim->parts[i].y, sim->parts[i].z, PartSwapBehavior::OCCUPY_SAME);
                    break;
                case PartProp::y:
                    sim->try_move(i, sim->parts[i].x, val_uint, sim->parts[i].z, PartSwapBehavior::OCCUPY_SAME);
                    break;
                case PartProp::z:
                    sim->try_move(i, sim->parts[i].x, sim->parts[i].y, val_uint, PartSwapBehavior::OCCUPY_SAME);
                    break;

                case PartProp::vx:
                    sim->parts[i].vx = static_cast<decltype(sim->parts[i].vx)>(val_float);
                    break;
                case PartProp::vy:
                    sim->parts[i].vy = static_cast<decltype(sim->parts[i].vy)>(val_float);
                    break;
                case PartProp::vz:
                    sim->parts[i].vz = static_cast<decltype(sim->parts[i].vz)>(val_float);
                    break;
                case PartProp::temp:
                    sim->heat_updates.push_back(PartHeatDelta{ i,
                        static_cast<decltype(sim->parts[i].temp)>(val_float) });
                    break;
                case PartProp::tmp1:
                    sim->parts[i].tmp1 = static_cast<decltype(sim->parts[i].tmp1)>(val_uint);
                    break;
                case PartProp::tmp2:
                    sim->parts[i].tmp2 = static_cast<decltype(sim->parts[i].tmp2)>(val_uint);
                    break;
                case PartProp::dcolor:
                    sim->parts[i].dcolor = static_cast<decltype(sim->parts[i].dcolor)>(val_uint);
                    break;
                case PartProp::invalid: break;
            }
        }

        return std::to_string(parts_updated) + " parts updated";
    };
}

#endif // INTERFACE_SIM_CONSOLE_COMMANDS_SET_H_
