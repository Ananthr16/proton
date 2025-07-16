#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./run-gl-uw <source_file.c> <unwind>" << std::endl;
        return 1;
    }

    std::string infile = argv[1];
    std::string unwind = argv[2];

    std::cout << "UNWIND=" << unwind << std::endl;

    std::string command = "cbmc --$BIT_WIDTH --unwind " + unwind +
                          " -D __VERIFIER_assert=orignial__VERIFIER_assert "
                          "-D __assert_fail=original__assert_fail --no-built-in-assertions "
                          "--stop-on-fail --object-bits 16 " + infile +
                          " --compact-trace --trace-show-function-calls";

    std::cout << command << std::endl;

    int ev = system(command.c_str());

    std::cout << "EXIT STATUS is " << ev << std::endl;

    if (ev == 10) {
        std::string home = std::getenv("HOME");
        std::filesystem::path logFilePath = std::filesystem::path(home) / "temp" / "x.txt";

        std::ofstream logFile(logFilePath, std::ios::app);
        if (logFile.is_open()) {
            logFile << infile << " RECURRENT STATE FOUND AT UNWIND " << unwind << std::endl;
        } else {
            std::cerr << "Failed to write to " << logFilePath << std::endl;
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