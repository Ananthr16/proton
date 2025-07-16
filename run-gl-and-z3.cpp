#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./run-gl-and-z3 <instrumented_file> <original_file>" << std::endl;
        return 1;
    }

    std::string file1 = argv[1]; 
    std::string file2 = argv[2]; 

    int GLRV = 0, Z3RV = 0, TRES = 0, NTRES = 0, EXCO = 0;
    std::string RESTXT = "DEFAULT";

    std::cout << R"(
* *                 CBMC 5.95.0 (n/a) 64-bit                * *
* *                 Copyright (C) 2001-2018                 * *
* *              Daniel Kroening, Edmund Clarke             * *
* * Carnegie Mellon University, Computer Science Department * *
* *                  kroening@kroening.com                  * *
* *        Protected in part by U.S. patent 7,225,417       * *
* *  *****************************************************  * *
* *                 Built with Llama                        * *
* *  *****************************************************  * *
)";

    std::string grepCmd = "grep -ql \"recurrent state\" " + file1;
    int grepResult = system(grepCmd.c_str());

    if (grepResult == 0) {
        std::string cmd = "timeout 3m run-one-bounty.sh " + file1 + " >& " + file1 + ".err";
        Z3RV = system(cmd.c_str()) / 256;

        if (GLRV == 10 || Z3RV == 10) {
            NTRES = 1;
            TRES = 10;
            std::string traceCmd = "$TOOL_DIR/parse_trace_vf.py " + file1 + ".err " + file2 + " $TOOL_DIR/$WITNESS_FILE $BIT_WIDTH";
            system(traceCmd.c_str());
        }

        std::cout << "NT-CHECKRESULT. GLRV=" << GLRV << ". Z3RV=" << Z3RV << ". NTRES=" << NTRES << std::endl;
    } else {
        std::cout << "MISSING RSA" << std::endl;
    }

    if (NTRES != 1) {
        std::string tCheckCmd = "timeout 2m t-check.sh " + file2 + " >& " + file2 + ".terr";
        TRES = system(tCheckCmd.c_str()) / 256;

        if (TRES != 0) {
            std::string llmCheck = "$TOOL_DIR/llm-t-check.sh " + file2 + " >& " + file2 + ".terr";
            TRES = system(llmCheck.c_str()) / 256;
            std::cout << "LLM_T_CHECK EXIT VAL " << TRES << std::endl;
        }

        std::cout << "T-CHECKRESULT. TRES=" << TRES << std::endl;
        EXCO = 0;
        RESTXT = "DEFAULT";
    }

    if (TRES == 0 && NTRES == 1) {
        RESTXT = "UNKNOWN";
        EXCO = 2;
    } else if (NTRES == 1 && TRES == 10) {
        RESTXT = "FALSE(termination)";
        EXCO = 1;
    } else if (TRES == 0) {
        RESTXT = "TRUE";
        EXCO = 0;
    } else if (Z3RV == 6) {
        EXCO = 4;
        RESTXT = "SOLVER INTERNAL ERROR";
    } else {
        RESTXT += " INCONCLUSIVE";
        EXCO = 3;
    }

    std::cout << "FILE=" << file1
              << " GLRES=" << GLRV
              << " Z3RES=" << Z3RV
              << " NTRES=" << NTRES
              << " TRES=" << TRES
              << " TNT-RESULT=" << RESTXT
              << " EXITCODE=" << EXCO << std::endl;

    return EXCO;
}