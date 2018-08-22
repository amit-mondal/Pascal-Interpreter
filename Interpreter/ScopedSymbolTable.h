/*
 * Symbol table class definition
 */

#ifndef SCOPEDSYMBOLTABLE_H
#define SCOPEDSYMBOLTABLE_H

#include "utils.h"
#include "Symbol.h"

class ScopedSymbolTable {
public:
    utils::OrderedMap<std::string, Symbol> symbols;
    ScopedSymbolTable(std::string scopeName, int scopeLevel, ScopedSymbolTable* enclosingScope);
    void define(Symbol* symbol);
    Symbol* lookup(std::string name, bool currScope = false);
    static void initBuiltIns();
    std::string toString() const;
    ScopedSymbolTable* enclosingScope;
    int scopeLevel;
    std::string name();
    static const int NUM_BUILTINS = 4;
    enum builtInSymbols {
			 INT = 0,
			 REAL,
			 STRING,
			 ANY
    };
    static BuiltInTypeSymbol* builtInsMap[NUM_BUILTINS];
private:
    std::string scopeName;
};

#endif
