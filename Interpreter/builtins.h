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
#include "CallStack.h"

#define BUILTIN(name) \
    DataVal builtIn_ ## name (CallStack* stack, std::vector<DataVal> args )

#define BUILTIN_ENTRY(name, returnType, ...) \
    { #name, { #name, &builtIn_ ## name, returnType, { __VA_ARGS__ }} }


BUILTIN(DUMP);
BUILTIN(PRINT);
BUILTIN(PRINTLN);
BUILTIN(SLEEP);
BUILTIN(STRMODIFY);
BUILTIN(PANIC);
BUILTIN(BIND);
BUILTIN(PARSEINT);
BUILTIN(INPUT);
BUILTIN(INT_TO_REAL);
BUILTIN(REAL_TO_INT);

namespace builtin {

    const bool initBuiltIns = ScopedSymbolTable::initBuiltIns();

    struct Fn {
	std::string name;
	std::function<DataVal(CallStack*, std::vector<DataVal>)> fn;
	Symbol* returnType;
	std::vector<ScopedSymbolTable::builtInSymbols> paramTypes;
    };

    const std::unordered_map<std::string, Fn> FUNCTIONS =
	{
	 BUILTIN_ENTRY(PARSEINT, GET_BUILT_IN_SYMBOL(INT), BUILT_IN_TYPE(STRING)),
	 BUILTIN_ENTRY(DUMP, nullptr, BUILT_IN_TYPE(ANY)),
	 BUILTIN_ENTRY(PRINT, nullptr, BUILT_IN_TYPE(STRING)),
	 BUILTIN_ENTRY(PRINTLN, nullptr, BUILT_IN_TYPE(STRING)),
	 BUILTIN_ENTRY(SLEEP, nullptr, BUILT_IN_TYPE(REAL)),
	 BUILTIN_ENTRY(STRMODIFY, nullptr, BUILT_IN_TYPE(STRING), BUILT_IN_TYPE(STRING),  BUILT_IN_TYPE(REAL)),
	 BUILTIN_ENTRY(PANIC, nullptr),
	 BUILTIN_ENTRY(BIND, nullptr, BUILT_IN_TYPE(STRING), BUILT_IN_TYPE(ANY)),
	 BUILTIN_ENTRY(INPUT, GET_BUILT_IN_SYMBOL(STRING)),
	 BUILTIN_ENTRY(INT_TO_REAL, GET_BUILT_IN_SYMBOL(REAL), BUILT_IN_TYPE(INT)),
	 BUILTIN_ENTRY(REAL_TO_INT, GET_BUILT_IN_SYMBOL(INT), BUILT_IN_TYPE(REAL))
	};

    void error(const std::string& err, const std::string& name);
}

#endif
