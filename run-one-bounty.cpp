// run-one-bounty.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "utils.h"   


static int run_gl(const std::string& file, const std::string& unwind, int timeout_sec) {
    std::cout << "RUNNING GL for unwind " << unwind << std::endl;
    const std::string cmd =
        "timeout " + std::to_string(timeout_sec) +
        " run-gl-uw.sh " + file + " " + unwind +
        " > " + file + ".err";
  
    return runCommand(cmd, false);
}


static int run_kissat(const std::string& file, const std::string& unwind, int timeout_sec) {
    std::cout << "RUNNING KISSAT for unwind " << unwind << std::endl;
    const std::string cmd =
        "timeout " + std::to_string(timeout_sec) +
        " run-kissat-uw.sh " + file + " " + unwind +
        " > " + file + ".err";
    return runCommand(cmd, false);
}


static int run_z3(const std::string& file, const std::string& unwind) {
    std::cout << "RUNNING Z3 for unwind " << unwind << std::endl;
    const std::string cmd =
        "run-z3-uw.sh " + file + " " + unwind +
        " > " + file + ".err";
    return runCommand(cmd, false);
}

int run_one_bounty(const std::string& file) {

    {
        const std::string grepCmd = "grep -ql 'recurrent state' " + file;
        int grepResult = runCommand(grepCmd, true);
        if (grepResult != 0) {
            std::cout << "RSA ABSENT" << std::endl;
            return 2; 
        }
    }

 
    const std::vector<int> unwinds = {2, 3, 4, 10, 12, 20, 40, 100, 1000};
    int max_time = 210;
    const auto t0 = std::chrono::steady_clock::now();

    auto elapsed = [&]() -> int {
        return static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - t0).count());
    };

    for (int uw : unwinds) {
        int remaining = max_time - elapsed();
        if (remaining <= 0) {
            
            int zstat = run_z3(file, std::to_string(uw));
            if (zstat == 10) {
                std::cout << "Termination found at unwind " << uw << " by Z3\n";
                return 10;
            }
        
            continue;
        }


        int glstat = run_gl(file, std::to_string(uw), remaining);

        if (glstat == 10) {
            std::cout << "Termination found at unwind " << uw << " by GL\n";
            return 10;
        }

        if (glstat == 6 || glstat == 137) {
            std::cout << "GLUCOSE error or OUT OF MEM on unwind " << uw
                      << ". exit_status=" << glstat << std::endl;
           
            max_time = 0;
            int zstat = run_z3(file, std::to_string(uw));
            if (zstat == 10) {
                std::cout << "Termination found at unwind " << uw << " by Z3\n";
                return 10;
            }
            continue;
        }

        if (glstat == 124) {
            std::cout << "GLUCOSE timed out on unwind " << uw << ".\n";
           
            max_time = 0;
            int zstat = run_z3(file, std::to_string(uw));
            if (zstat == 10) {
                std::cout << "Termination found at unwind " << uw << " by Z3\n";
                return 10;
            }
            continue;
        }

      
        int kistat = run_kissat(file, std::to_string(uw), remaining);
        if (kistat == 10) {
            std::cout << "KISSAT found termination at unwind " << uw << std::endl;
            return 10;
        }
    }

    std::cout << "run-one-bounty completed successfully within the time limit." << std::endl;
    return 1; 
}