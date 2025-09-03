#include <iostream>
#include <string>
#include <filesystem>
#include "utils.h"   

int run_z3_uw(const std::string& infile, const std::string& uw) {
    std::cout << "UNWIND=" << uw << std::endl;

    const char* bit_width_env = std::getenv("BIT_WIDTH");
    std::string bit_width = bit_width_env ? bit_width_env : "";

    std::string command = "cbmc --" + bit_width +
        " --unwind " + uw +
        " -D __VERIFIER_assert=orignial__VERIFIER_assert" +
        " -D __assert_fail=original__assert_fail" +
        " --no-built-in-assertions --stop-on-fail --object-bits 16 " +
        infile +
        " --compact-trace --trace-show-function-calls --z3";

    std::cout << command << std::endl;

    int ev = runCommand(command);

    if (ev == 10) {
        std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
        std::string logCmd = "echo " + infile + " RECURRENT STATE FOUND AT UNWIND " + uw + 
                             " >> " + home + "/temp/x.txt";
        runCommand(logCmd);  
        return ev;
    }

    if (ev == 6) {
        std::cout << infile << " RECURRENT STATE NOFOUND AT UNWIND " << uw 
                  << ". Z3 internal error\n";
        return ev;
    }

    if (ev == 137) {
        std::cout << infile << " RECURRENT STATE NOFOUND AT UNWIND " << uw 
                  << ". OUT OF MEMORY\n";
        return ev;
    }

    std::cout << infile << " RECURRENT STATE NOTFOUND AT UNWIND " << uw 
              << " echo " << ev << std::endl;
    return ev;
}