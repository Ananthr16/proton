#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <filesystem>
#include <cstdio>
#include <sstream>

using namespace std;

int run_gl_dummy(const string&, const string&, int) {
    return 137;
}

int run_gl(const string& file, const string& unwind, int timeout_sec) {
    cout << "RUNNING GL for unwind " << unwind << endl;
    string command = "timeout " + to_string(timeout_sec) + " run-gl-uw.sh " + file + " " + unwind + " > " + file + ".err";
    int status = system(command.c_str()) / 256;

    if (status == 10) {
        cout << "FALSE(termination)" << endl;
        exit(status);
    }
    return status;
}

int run_ki(const string& file, const string& unwind, int timeout_sec) {
    cout << "RUNNING KISSAT for unwind " << unwind << endl;
    string command = "timeout " + to_string(timeout_sec) + " run-kissat-uw.sh " + file + " " + unwind + " > " + file + ".err";
    int status = system(command.c_str()) / 256;

    if (status == 10) {
        cout << "FALSE(termination)" << endl;
        exit(status);
    }
    return status;
}

int run_z3(const string& file, const string& unwind) {
    cout << "RUNNING Z3 for unwind " << unwind << endl;
    string command = "run-z3-uw.sh " + file + " " + unwind + " > " + file + ".err";
    int status = system(command.c_str()) / 256;

    if (status == 10) {
        cout << "FALSE(termination)" << endl;
        exit(status);
    }

    if (status == 6 || status == 137) {
        cout << "Z3 error or OUT OF MEM on unwind " << unwind << "." << endl;
        exit(status);
    }

    return status;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./run-one-bounty <filename>" << endl;
        return 1;
    }

    string file = argv[1];

    if (system(("grep -ql 'recurrent state' " + file).c_str()) != 0) {
        cout << "RSA ABSENT" << endl;
        return 2;
    }

    vector<int> numbers = {2, 3, 4, 10, 12, 20, 40, 100, 1000};
    int max_time = 210;
    auto start_time = chrono::steady_clock::now();

    auto elapsed_time = [&]() {
        return chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start_time).count();
    };

    for (int num : numbers) {
        int remaining_time = max_time - elapsed_time();
        if (remaining_time <= 0) {
            run_z3(file, to_string(num));
            continue;
        }

        int status = run_gl_dummy(file, to_string(num), remaining_time);

        if (status == 6 || status == 137) {
            cout << "GLUCOSE error or OUT OF MEM on unwind " << num << ". exit_status=" << status << endl;
            max_time = 0;
            run_z3(file, to_string(num));
            continue;
        }

        if (status == 124) {
            cout << "GLUCOSE timed out on unwind " << num << "." << endl;
            max_time = 0;
            run_z3(file, to_string(num));
            continue;
        }
    }

    cout << "run-one-bounty completed successfully within the time limit." << endl;
    return 1;
}