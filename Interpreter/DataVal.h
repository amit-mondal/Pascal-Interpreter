#ifndef DATAVAL_H
#define DATAVAL_H

#include <iostream>
#include <cstdint>
#include "Allocator.h"

class RecordDecl;


#define DATAVAL_COMPARISON_BODY(OPERATOR)				\
    if (lhs.type != rhs.type) utils::fatalError				\
				  ("Cannot use binary operator " #OPERATOR " to compare types " + to_string(lhs.type) + " and " + to_string(rhs.type)); \
    switch(lhs.type) {							\
    case DataVal::D_STRING: return *((std::string*) lhs.data) OPERATOR *((std::string*) rhs.data); \
    case DataVal::D_INT: return *((int*) lhs.data) OPERATOR *((int*) rhs.data); \
    case DataVal::D_REAL: return *((double*) lhs.data) OPERATOR *((double*) rhs.data); \
    default: return false;		\
    }

#define DATAVAL_OPERATION_BODY(OPERATOR)                                \
    if (lhs.type != rhs.type) utils::fatalError				\
				  ("Cannot use numeric operation " #OPERATOR " on types " + to_string(lhs.type) + " and " + to_string(rhs.type)); \
    switch(lhs.type) {		           				\
    case DataVal::D_STRING: return DataVal::allocator.allocate( \
							       *((std::string*) lhs.data) OPERATOR *((std::string*) rhs.data)); \
    case DataVal::D_INT: return DataVal::allocator.allocate(*((int*) lhs.data) OPERATOR *((int*) rhs.data)); \
    case DataVal::D_REAL: return DataVal::allocator.allocate(*((double*) lhs.data) OPERATOR *((double*) rhs.data)); \
    default: { utils::fatalError("Cannot use binary operator " \
				 #OPERATOR " on non-numeric type " + to_string(lhs.type)); return DataVal(); } \
    }

#define DATAVAL_NUMERIC_OPERATION_BODY(OPERATOR)	\
    if (lhs.type != rhs.type) utils::fatalError				\
				  ("Cannot use numeric operation " #OPERATOR " on types " + to_string(lhs.type) + " and " + to_string(rhs.type)); \
    switch(lhs.type) {							\
    case DataVal::D_INT: return DataVal::allocator.allocate(*((int*) lhs.data) OPERATOR *((int*) rhs.data)); \
    case DataVal::D_REAL: return DataVal::allocator.allocate(*((double*) lhs.data) OPERATOR *((double*) rhs.data)); \
    default: { utils::fatalError("Cannot use binary operator " \
				 #OPERATOR " on non-numeric type " + to_string(lhs.type)); return DataVal(); } \
    }

#define DATAVAL_GET_VAL(TYPE, VALUE) \
    * (( TYPE * ) VALUE)

#define DATAVAL_GET_PTR(TYPE, VALUE) \
    (( TYPE * ) VALUE.data)

struct DataVal {
    static Allocator allocator;
    
    void* data;
    size_t listIdx;
    
    enum Type {
        D_STRING,
        D_INT,
        D_REAL,
	D_COMP,
	D_NONE,
    };
    Type type;
    
    RecordDecl* recordDecl = nullptr;
    DataVal();
    std::string toString() const;
    bool toBool() const;
    bool isNumeric() const;
    void freeData();
    void memoryCheck(DataVal::Type type);
    friend std::ostream &operator<< (std::ostream& os, DataVal const& token);
    friend bool operator==(const DataVal& lhs, const DataVal& rhs);
    friend bool operator!=(const DataVal& lhs, const DataVal& rhs);
    friend bool operator<(const DataVal& lhs, const DataVal& rhs);
    friend bool operator>(const DataVal& lhs, const DataVal& rhs);
    friend DataVal operator+(const DataVal& lhs, const DataVal& rhs);
    friend DataVal operator-(const DataVal& lhs, const DataVal& rhs);
    friend DataVal operator*(const DataVal& lhs, const DataVal& rhs);
    friend DataVal operator/(const DataVal& lhs, const DataVal& rhs);
};

#endif
