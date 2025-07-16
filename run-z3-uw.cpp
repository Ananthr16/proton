#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./run-z3-uw.cpp <input_file> <unwind_value>\n";
        return 1;
    }

    std::string infile = argv[1];
    std::string uw = argv[2];

    std::cout << "UNWIND=" << uw << std::endl;

    // Get BT_Width from the environment variable
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

    int ev = std::system(command.c_str());
    ev = WEXITSTATUS(ev); // to normalise the return code

    if (ev == 10) {
        std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
        std::string logCmd = "echo " + infile + " RECURRENT STATE FOUND AT UNWIND " + uw + " >> " + home + "/temp/x.txt";
        std::system(logCmd.c_str());
        return ev;
    }

    if (ev == 6) {
        std::cout << infile << " RECURRENT STATE NOFOUND AT UNWIND " << uw << ". Z3 internal error\n";
        return ev;
    }

    if (ev == 137) {
        std::cout << infile << " RECURRENT STATE NOFOUND AT UNWIND " << uw << ". OUT OF MEMORY\n";
        return ev;
    }

    std::cout << infile << " RECURRENT STATE NOTFOUND AT UNWIND " << uw << " echo " << ev << std::endl;
    return ev;
}