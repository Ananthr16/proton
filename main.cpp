#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>

#include "Options.h"
#include "parse.h"
#include "Runner.h"  // <- NEW include for full pipeline

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./main <property_file.prp> <source_file.c>" << std::endl;
        return 1;
    }

    std::string propFile = argv[1];
    std::string sourceFile = argv[2];

    // Parse property file and set options
    parsePropertyFile(propFile);

    // Print selected flags for debug info
    Options* opts = Options::getInstance();
    std::cout << "Selected flags: " << opts->getOptionFlags() << std::endl;

    // Run full PROTON tool pipeline using Runner
    Runner runner;
    return runner.run(sourceFile);
}