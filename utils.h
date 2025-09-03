#ifndef UTILS_H
#define UTILS_H

#include <string>

// Runs a shell command.
// - cmd: the command to run
// - silent: if true, suppresses stdout/stderr
// Returns: normalized exit code (0 = success)
int runCommand(const std::string& cmd, bool silent = false);

#endif