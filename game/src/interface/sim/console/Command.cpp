#include "Command.h"
#include "../../../simulation/Simulation.h"
#include "../../../util/colored_text.h"
#include "./commands/set.h"

using namespace commands;
using namespace text_format;

void commands::setup_commands() {
    cmd_list.emplace_back("help",
        F_SKYBLUE + "!help " + F_RESET + "- List all commands",
        [](Simulation * sim, const std::vector<std::string> &tokens) {
            return help_text;
        });

    cmd_list.emplace_back("set",
        F_SKYBLUE + "!set " + F_RESET + "[prop] [all|type]" + F_GOLD + " [value]" + F_RESET +
        " - Set property on all (or a given type) of particles. Example:\n"
        "   To set all water particles to a temperature of 0C, run\n"
        + F_YELLOW + "   !set temp watr 0C",
        commands::cmd_set);

    // Generate help text
    for (auto i = 0; i < cmd_list.size(); i++) {
        help_text += cmd_list[i].help;
        if (i != cmd_list.size() - 1)
            help_text += '\n';
    }
}
