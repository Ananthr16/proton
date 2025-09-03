#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include "utils.h"   

int run_gl_uw(const std::string& infile, const std::string& unwind) {
    std::cout << "UNWIND=" << unwind << std::endl;

    const char* bw_env = std::getenv("BIT_WIDTH");
    std::string bitWidth = bw_env ? std::string("--object-bits ") + bw_env : "--object-bits 16";

    std::string command = "cbmc " + bitWidth + " --unwind " + unwind +
                          " -D __VERIFIER_assert=orignial__VERIFIER_assert "
                          "-D __assert_fail=original__assert_fail --no-built-in-assertions "
                          "--stop-on-fail " + infile +
                          " --compact-trace --trace-show-function-calls";

    std::cout << command << std::endl;

  
    int ev = runCommand(command);

    std::cout << "EXIT STATUS is " << ev << std::endl;

    if (ev == 10) {
        const char* home = std::getenv("HOME");
        if (home) {
            std::filesystem::path logFilePath = std::filesystem::path(home) / "temp" / "x.txt";
            std::ofstream logFile(logFilePath, std::ios::app);
            if (logFile.is_open()) {
                logFile << infile << " RECURRENT STATE FOUND AT UNWIND " << unwind << std::endl;
            } else {
                std::cerr << "Failed to write to " << logFilePath << std::endl;
            }
        }
        return ev;
    }

    if (ev == 137) {
        std::cout << infile << " RECURRENT STATE NOFOUND AT UNWIND " << unwind
                  << ". OUT OF MEMORY" << std::endl;
        return ev;
    }

    std::cout << infile << " RECURRENT STATE NOTFOUND AT UNWIND " << unwind
              << " echo " << ev << std::endl;
    return ev;
}