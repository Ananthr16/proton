#include "Options.h"
#include <fstream>
#include <regex>
#include <iostream>
#include <string>

void parsePropertyFile(const std::string& propFilePath) {
    std::ifstream file(propFilePath);
    if (!file.is_open()) {
        std::cerr << "Could not open property file: " << propFilePath << std::endl;
        return;
    }

    std::string line;
    std::regex fullRegex(R"(CHECK\(init\((\w+)\(\)\),LTL\((.+)\)\))");
    std::smatch match;

    Options* opts = Options::getInstance();

    while (std::getline(file, line)) {
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end()); // remove whitespace

        if (std::regex_match(line, match, fullRegex)) {
            std::string entryFunc = match[1];
            std::string propExpr = match[2];

            opts->setEntry(entryFunc);

            if (std::regex_match(propExpr, std::regex(R"(G!label\((\w+)\))"))) {
                opts->setProp(PropertyType::Label);
                opts->setLabel(entryFunc);
            } else if (std::regex_match(propExpr, std::regex(R"(G!call\((\w+)\(\)\))"))) {
                opts->setProp(PropertyType::UnreachCall);
                opts->setFailFunction(std::regex_replace(propExpr, std::regex(R"(G!call\((\w+)\(\)\))"), "$1"));
            } else if (propExpr == "Gassert") {
                opts->setProp(PropertyType::UnreachCall);
            } else if (std::regex_match(propExpr, std::regex(R"(Gvalid-(free|deref|memtrack))"))) {
                opts->setProp(PropertyType::Memsafety);
            } else if (propExpr == "Gvalid-memcleanup") {
                opts->setProp(PropertyType::Memcleanup);
            } else if (propExpr == "G!overflow") {
                opts->setProp(PropertyType::Overflow);
            } else if (propExpr == "Fend") {
                opts->setProp(PropertyType::Termination);
            } else {
                std::cerr << "Unknown property: " << propExpr << std::endl;
            }
        }
    }

    file.close();