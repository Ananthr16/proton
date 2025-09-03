#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "utils.h"  

int run_kissat_uw(const std::string& infile, const std::string& unwind) {
    const char* bit_width_env = std::getenv("BIT_WIDTH");
    std::string bitWidth = bit_width_env ? bit_width_env : "";

    std::cout << "UNWIND=" << unwind << std::endl;

    std::string command = "cbmc --external-sat-solver kissat --" + bitWidth +
        " --unwind " + unwind +
        " -D __VERIFIER_assert=orignial__VERIFIER_assert" +
        " -D __assert_fail=original__assert_fail" +
        " --no-built-in-assertions --stop-on-fail --object-bits 16 " +
        infile +
        " --compact-trace --trace-show-function-calls";

    std::cout << command << std::endl;

    int ev = runCommand(command); 
    std::cout << "EXIT STATUS is " << ev << std::endl;

    if (ev == 10) {
        std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
        std::string logPath = home + "/temp/x.txt";
        std::ofstream logFile(logPath, std::ios::app);
        if (logFile.is_open()) {
            logFile << infile << " RECURRENT STATE FOUND AT UNWIND " << unwind << std::endl;
        } else {
            std::cerr << "Could not open log file: " << logPath << std::endl;
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