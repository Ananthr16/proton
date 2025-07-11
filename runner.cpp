// Runner.cpp
#include "Runner.h"
#include "Options.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <sstream>

int Runner::run(const std::string& sourceFilePath) {
    toolDir = std::filesystem::current_path();
    fileName = getFileName(sourceFilePath);
    Options* opts = Options::getInstance();

    // only support termination for now
    if (opts->getProp() != PropertyType::Termination) {
        std::cerr << "UNSUPPORTED PROPERTY" << std::endl;
        return 6;
    }

    property = opts->getOptionFlags() + " --no-assertions --no-self-loops-to-assumptions";

    // make temp dir
    char tempTemplate[] = "/tmp/proton-XXXXXX";
    char* tempPath = mkdtemp(tempTemplate);
    if (!tempPath) {
        std::cerr << "Failed to create temp directory." << std::endl;
        return 7;
    }
    tempDir = tempPath;

    std::filesystem::copy(sourceFilePath, tempDir + "/" + fileName);
    std::filesystem::current_path(tempDir);

    if (!runBracer(fileName)) {
        std::cerr << "Bracer failed." << std::endl;
        return 8;
    }

    std::string instrumentInput = determineInstrumentInput();

    if (!runInstrumenter(instrumentInput)) {
        std::cerr << "Instrumenter failed." << std::endl;
        return 9;
    }

    std::string finalFile = findInstrumentedFile(fileName);
    int ec = runZ3(finalFile, fileName);

    std::filesystem::current_path(toolDir);
    cleanup();

    switch (ec) {
        case 0: std::cout << "TRUE" << std::endl; return 0;
        case 1: std::cout << "FALSE(termination)" << std::endl; return 0;
        case 2: std::cout << "UNKNOWN" << std::endl; return 10;
        case 3: std::cout << "INCONCLUSIVE" << std::endl; return 10;
        default: std::cout << "INTERNAL-ERROR" << std::endl; return 10;
    }
}

bool Runner::runBracer(const std::string& filePath) {
    std::string command = "bracer -in " + filePath + " -c > OutFile.txt";
    return system(command.c_str()) == 0;
}

std::string Runner::determineInstrumentInput() {
    std::ifstream outFile("OutFile.txt");
    std::string line;
    while (std::getline(outFile, line)) {
        if (line.find("brace_it called") != std::string::npos) {
            for (const auto& f : std::filesystem::directory_iterator(".")) {
                if (f.path().filename().string().find("braced.c") != std::string::npos) {
                    return f.path().filename();
                }
            }
        }
    }
    return fileName;
}

bool Runner::runInstrumenter(const std::string& filePath) {
    std::string command = "instrumenter -in " + filePath + " -c";
    return system(command.c_str()) == 0;
}

std::string Runner::findInstrumentedFile(const std::string& origFile) {
    for (const auto& f : std::filesystem::directory_iterator(".")) {
        if (f.path().filename().string().find("_instrumented.c") != std::string::npos) {
            return f.path().filename();
        }
    }
    return origFile;
}

int Runner::runZ3(const std::string& instrumentedFile, const std::string& originalFile) {
    std::string command = toolDir + "/run-gl-and-z3.sh " + instrumentedFile + " " + originalFile;
    return system(command.c_str()) / 256; // convert shell exit to C++ return code
}

std::string Runner::getFileName(const std::string& path) {
    return std::filesystem::path(path).filename();
}

void Runner::cleanup() {
    try {
        std::filesystem::remove_all(tempDir);
    } catch (...) {
        std::cerr << "Warning: failed to remove temp directory." << std::endl;
    }
}