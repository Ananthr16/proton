// Runner.h
#ifndef RUNNER_H
#define RUNNER_H

#include <string>

class Runner {
public:
    
    int run(const std::string& sourceFilePath);

private:
    std::string toolDir;
    std::string tempDir;
    std::string fileName;
    std::string property;

    
    bool runBracer(const std::string& filePath);
    std::string determineInstrumentInput();
    bool runInstrumenter(const std::string& filePath);
    std::string findInstrumentedFile(const std::string& origFile);
    int runZ3(const std::string& instrumentedFile, const std::string& originalFile);
    std::string getFileName(const std::string& path);
    void cleanup();
};

#endif 