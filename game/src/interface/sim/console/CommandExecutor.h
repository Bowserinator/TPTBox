#ifndef INTERFACE_SIM_CONSOLE_COMMANDEXECUTOR_H_
#define INTERFACE_SIM_CONSOLE_COMMANDEXECUTOR_H_

#include <string>

class Simulation;

/**
 * @brief Execute a console command
 * 
 * @param sim Simulation context
 * @param command Command to run
 * @return std::string Result
 */
std::string executeCommand(Simulation * sim, const std::string &command);

#endif // INTERFACE_SIM_CONSOLE_COMMANDEXECUTOR_H_
