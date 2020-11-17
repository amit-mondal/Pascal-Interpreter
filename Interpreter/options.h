#ifndef OPTIONS_H
#define OPTIONS_H

#define DEFINE_CMD_LINE_OPT(input, optionName, shortStr, longStr)	\
    options::optionName = (input.cmdOptionExists(shortStr) || input.cmdOptionExists(longStr))

namespace options {
    extern bool printTokens;
    extern bool dumpVars;
    extern bool showST;
    extern bool showConditions;
    extern bool staticTypeChecking;
    extern bool showAllocations;
}

#endif
