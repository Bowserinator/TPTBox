#ifndef INTERFACE_SIM_CONSOLE_COMMAND_H_
#define INTERFACE_SIM_CONSOLE_COMMAND_H_

#include <vector>
#include <string>
#include <functional>

class Simulation;

namespace commands {
    /** A ! command, ie !set would be ConsoleCommand("set", [](...) {... }) */
    struct ConsoleCommand {
        using ExecF = std::function<std::string(Simulation * sim, const std::vector<std::string> &tokens)>;

        /**
         * @brief Construct a new Console Command object
         * @param match Name of command without leading !
         * @param help Help text
         * @param exec Function to execute, return return message
         */
        ConsoleCommand(const std::string &match, const std::string &help, const ExecF exec) :
            match(match), help(help), exec(exec) {};

        const std::string match; // Name of command without leading !
        const std::string help;
        const ExecF exec;
    };

    inline std::vector<ConsoleCommand> cmd_list;
    inline std::string help_text = "";

    /** Inits cmd_list */
    extern void setup_commands();
} // namespace commands

#endif // INTERFACE_SIM_CONSOLE_COMMAND_H_
