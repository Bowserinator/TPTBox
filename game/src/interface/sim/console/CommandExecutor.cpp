#include "CommandExecutor.h"
#include "Command.h"
#include "../../../simulation/Simulation.h"

#include <sstream>
#include <vector>

std::string executeCommand(Simulation * sim, const std::string &command) {
    if (!command.length())
        return "Error: empty command";
    if (command[0] != '!')
        return "Error: expected ! for command"; // TODO: future scripting commands won't need this

    if (command[0] == '!') {
        auto ss = std::stringstream{command};
        std::vector<std::string> tokens;
        for (std::string tok; std::getline(ss, tok, ' ');)
           tokens.push_back(tok);

        if (tokens[0].length() == 1)
            return "Error: expected name after ! for command";

        tokens[0] = tokens[0].substr(1);
        for (auto &cmd : commands::cmd_list) {
            if (tokens[0] == cmd.match)
                return cmd.exec(sim, tokens);
        }
        return "Unknown command: " + tokens[0];
    }

    return "Critical bug has occurred";
}
