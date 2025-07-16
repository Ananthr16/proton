#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <fstream>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./run_kissat_uw <infile> <unwind>" << std::endl;
        return 1;
    }

    std::string infile = argv[1];
    std::string unwind = argv[2];
    const char* bit_width = std::getenv("BIT_WIDTH");
    std::string bitWidth = bit_width ? bit_width : "";

    std::cout << "UNWIND=" << unwind << std::endl;

    std::stringstream cmdLog;
    cmdLog << "cbmc --external-sat-solver kissat --" << bitWidth
           << " --unwind " << unwind
           << " -D __VERIFIER_assert=orignial__VERIFIER_assert"
           << " -D __assert_fail=original__assert_fail"
           << " --no-built-in-assertions --stop-on-fail --object-bits 16 "
           << infile
           << " --trace --trace-show-function-calls";
    std::cout << cmdLog.str() << std::endl;

    std::stringstream cmdExec;
    cmdExec << "cbmc --external-sat-solver kissat --" << bitWidth
            << " --unwind " << unwind
            << " -D __VERIFIER_assert=orignial__VERIFIER_assert"
            << " -D __assert_fail=original__assert_fail"
            << " --no-built-in-assertions --stop-on-fail --object-bits 16 "
            << infile
            << " --compact-trace --trace-show-function-calls";

    int ev = std::system(cmdExec.str().c_str());
    std::cout << "EXIT STATUS is " << ev << std::endl;

    if (ev == 10) {
        std::string home = std::getenv("HOME") ? std::getenv("HOME") : "";
        std::string logPath = home + "/temp/x.txt";
        std::ofstream logFile(logPath, std::ios::app);
        if (logFile.is_open()) {
            logFile << infile << " RECURRENT STATE FOUND AT UNWIND " << unwind << std::endl;
            logFile.close();
        }
        return ev;
    }

    if (ev == 137) {
        std::cout << infile << " RECURRENT STATE NOFOUND AT UNWIND " << unwind << ". OUT OF MEMORY" << std::endl;
        return ev;
    }

    std::cout << infile << " RECURRENT STATE NOTFOUND AT UNWIND " << unwind << " echo " << ev << std::endl;
    return ev;
}