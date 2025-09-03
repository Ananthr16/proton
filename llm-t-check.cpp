// llm_check.cpp
#include "llm_check.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

int runLLMCheck(const std::string& inputFile) {
    const std::string toolDir = std::filesystem::current_path();
    const std::string venvSourceDir = toolDir + "/venv-proton";
    const std::string venvTargetDir = toolDir + "/venv";
    const std::string modelFile = "Llama-iproton.gguf";
    const std::string pythonScript = "check_ter.py";

   
    try {
        std::filesystem::create_directory(venvTargetDir);
        std::filesystem::copy(venvSourceDir, venvTargetDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
        std::filesystem::permissions(venvTargetDir, std::filesystem::perms::all);
    } catch (std::exception& e) {
        std::cerr << "Error setting up virtual environment: " << e.what() << std::endl;
        return 1;
    }

    // Create logs and tmp directories
    std::filesystem::create_directory("tmp");
    std::filesystem::permissions("tmp", std::filesystem::perms::all);
    std::filesystem::create_directory("logs");
    std::filesystem::permissions("logs", std::filesystem::perms::all);

    // Getting the path
    std::string pyBin;
    {
        FILE* pipe = popen("which python3", "r");
        if (!pipe) return 22;
        char buffer[256];
        if (fgets(buffer, 256, pipe)) {
            pyBin = std::string(buffer);
            pyBin.erase(pyBin.find_last_not_of("\n") + 1);
        }
        pclose(pipe);
    }

    if (pyBin.empty()) {
        std::cerr << "python3 not found" << std::endl;
        return 22;
    }

   
    system(("ln -sf " + pyBin + " " + venvTargetDir + "/bin/python").c_str());
    system(("ln -sf " + pyBin + " " + venvTargetDir + "/bin/python3").c_str());
    system(("ln -sf " + venvTargetDir + "/lib " + venvTargetDir + "/lib64").c_str());

    // Fixing the paths 
    std::string fixVenvCmd = "python3 virtualenv_tools.py --update-path auto " + venvTargetDir;
    if (system(fixVenvCmd.c_str()) != 0) {
        std::cerr << "Failed to fix virtualenv paths" << std::endl;
        return 25;
    }

    // check_ter.py
    std::string command = "source " + venvTargetDir + "/bin/activate && " +
                          "python3 " + pythonScript +
                          " --in-file $BM_DIR/" + inputFile +
                          " --gguf " + modelFile + " >& /dev/null";


    int result = system(("bash -c \"" + command + "\"").c_str());
    return result / 256; 
}