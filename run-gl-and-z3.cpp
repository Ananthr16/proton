#include <iostream>
#include <string>
#include "utils.h"

int run_one_bounty(const std::string& file);

int run_gl_and_z3(const std::string& instrumented_file, const std::string& original_file) {
    int GLRV = 0, Z3RV = 0, TRES = 0, NTRES = 0, EXCO = 0;
    std::string RESTXT = "DEFAULT";

    std::cout << R"(* *                 CBMC 5.95.0 (n/a) 64-bit                * *
* *                 Copyright (C) 2001-2018                 * *
* *              Daniel Kroening, Edmund Clarke             * *
* * Carnegie Mellon University, Computer Science Department * *
* *                  kroening@kroening.com                  * *
* *        Protected in part by U.S. patent 7,225,417       * *
* *  *****************************************************  * *
* *                 Built with Llama                        * *
* *  *****************************************************  * *)" << std::endl;

    // grep check
    std::string grepCmd = "grep -ql \"recurrent state\" " + instrumented_file;
    int grepResult = runCommand(grepCmd);

    if (grepResult == 0) {
        Z3RV = run_one_bounty(instrumented_file);

        if (GLRV == 10 || Z3RV == 10) {
            NTRES = 1;
            TRES = 10;

            std::string traceCmd =
                "$TOOL_DIR/parse_trace_vf.py " + instrumented_file + ".err " +
                original_file + " $TOOL_DIR/$WITNESS_FILE $BIT_WIDTH";

            runCommand(traceCmd);
        }

        std::cout << "NT-CHECKRESULT. GLRV=" << GLRV
                  << ". Z3RV=" << Z3RV
                  << ". NTRES=" << NTRES << std::endl;
    } else {
        std::cout << "MISSING RSA" << std::endl;
    }

    if (NTRES != 1) {
        std::string tCheckCmd = "timeout 2m t-check.sh " + original_file + " >& " + original_file + ".terr";
        TRES = runCommand(tCheckCmd);

        if (TRES != 0) {
            std::string llmCheck = "$TOOL_DIR/llm-t-check.sh " + original_file + " >& " + original_file + ".terr";
            TRES = runCommand(llmCheck);
            std::cout << "LLM_T_CHECK EXIT VAL " << TRES << std::endl;
        }

        std::cout << "T-CHECKRESULT. TRES=" << TRES << std::endl;
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
        RESTXT = "SOLVER INTERNAL ERROR";
        EXCO = 4;
    } else {
        RESTXT += " INCONCLUSIVE";
        EXCO = 3;
    }

    std::cout << "FILE=" << instrumented_file
              << " GLRES=" << GLRV
              << " Z3RES=" << Z3RV
              << " NTRES=" << NTRES
              << " TRES=" << TRES
              << " TNT-RESULT=" << RESTXT
              << " EXITCODE=" << EXCO << std::endl;

    return EXCO;
}