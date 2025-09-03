#include "utils.h"
#include <iostream>
#include <cstdlib>

int runCommand(const std::string& cmd, bool silent) {
    std::string wrappedCmd = cmd;
    if (silent) {
        wrappedCmd += " >& /dev/null";
    }

    std::cout << "[RUN] " << wrappedCmd << std::endl;

    int result = std::system(wrappedCmd.c_str());
    if (result == -1) {
        std::cerr << "[ERROR] Failed to execute: " << cmd << std::endl;
        return 127; // consistent "failed to exec" code
    }
    return result / 256;  // normalize exit code
}