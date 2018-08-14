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
    void initBuiltIns();
    std::string toString() const;
    ScopedSymbolTable* enclosingScope;
    int scopeLevel;
    std::string name();
private:
    std::string scopeName;
};

#endif
