#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "utils.h"
#include "Symbol.h"
#include "ScopedSymbolTable.h"
#include "DataVal.h"

#define BUILTIN(name) \
    DataVal builtIn_ ## name (std::vector<DataVal> args )

#define BUILTIN_ENTRY(name, returnType, ...) \
    { #name, { #name, &builtIn_ ## name, returnType, { __VA_ARGS__ }} }


BUILTIN(DUMP);
BUILTIN(PRINT);
BUILTIN(PRINTLN);
BUILTIN(SLEEP);
BUILTIN(STRMODIFY);

namespace builtin {

    struct Fn {
	std::string name;
	std::function<DataVal(std::vector<DataVal>)> fn;
	Symbol* returnType;
	std::vector<ScopedSymbolTable::builtInSymbols> paramTypes;
    };

    const std::unordered_map<std::string, Fn> FUNCTIONS =
	{
	 BUILTIN_ENTRY(DUMP, nullptr, BUILT_IN_TYPE(ANY)),
	 BUILTIN_ENTRY(PRINT, nullptr, BUILT_IN_TYPE(STRING)),
	 BUILTIN_ENTRY(PRINTLN, nullptr, BUILT_IN_TYPE(STRING)),
	 BUILTIN_ENTRY(SLEEP, nullptr, BUILT_IN_TYPE(REAL)),
	 BUILTIN_ENTRY(STRMODIFY, nullptr, BUILT_IN_TYPE(STRING), BUILT_IN_TYPE(STRING),  BUILT_IN_TYPE(REAL))
	};

    void error(const std::string& err, const std::string& name);
}

#endif
