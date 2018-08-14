#ifndef SYMBOL_H
#define SYMBOL_H

/*
 * Symbol class definition
 */

#include "utils.h"

class Symbol {
public:
    std::string name;
    Symbol* type;
    std::string category;
    Symbol(std::string name);
    Symbol(std::string name, Symbol* type);
    enum SymbolType {
	       S_BUILTIN,
	       S_USERDEF,
	       S_VAR,
	       S_PROCSYMBOL
    };
    virtual std::string toString() const = 0;
    virtual SymbolType stype() const = 0;
};


/*
 * Built-in symbol subclass
 */

class BuiltInTypeSymbol: public Symbol {
public:
    BuiltInTypeSymbol(std::string name);
    virtual SymbolType stype() const;
    virtual std::string toString() const;
    friend bool operator==(const BuiltInTypeSymbol& lhs,
			   const BuiltInTypeSymbol& rhs);
};

class UserDefinedTypeSymbol: public Symbol {
 public:
    UserDefinedTypeSymbol(std::string name);
    virtual SymbolType stype() const;
    virtual std::string toString() const;
    friend bool operator==(const UserDefinedTypeSymbol& lhs,
			   const UserDefinedTypeSymbol& rhs);
};


/*
 * Variable symbol subclass
 */

class VarSymbol: public Symbol {
public:
    VarSymbol(std::string name, Symbol* type);
    virtual SymbolType stype() const;
    virtual std::string toString() const;
};

/*
 * Procedure symbol subclass
 */

class ProcedureSymbol: public Symbol {
public:
    ProcedureSymbol(std::string name, std::vector<Symbol*>* params);
    ProcedureSymbol(std::string name);
    virtual SymbolType stype() const;
    virtual std::string toString() const;
    std::vector<Symbol*>* params;
};

#endif
