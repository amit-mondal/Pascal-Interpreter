#ifndef DATAVAL_H
#define DATAVAL_H

#include <iostream>

class RecordDecl;


#define DATAVAL_COMPARISON_BODY(OPERATOR)				\
    if (lhs.type != rhs.type) utils::fatalError				\
				  ("Cannot use binary operator" #OPERATOR "to compare types " + to_string(lhs.type) + " and " + to_string(rhs.type)); \
    switch(lhs.type) {							\
    case DataVal::D_STRING: return *((string*) lhs.data) OPERATOR *((string*) rhs.data); \
    case DataVal::D_INT: return *((int*) lhs.data) OPERATOR *((int*) rhs.data); \
    case DataVal::D_REAL: return *((double*) lhs.data) OPERATOR *((double*) rhs.data); \
    default: return false;						\
    }									\

#define DATAVAL_GET_VAL(TYPE, VALUE) \
    * (( TYPE * ) VALUE)

struct DataVal {
    void* data;
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
    DataVal(double val);
    DataVal(int val);
    DataVal(std::string val);
    std::string toString() const;
    bool toBool() const;
    bool isNumeric() const;
    void freeData();
    friend std::ostream &operator<< (std::ostream& os, DataVal const& token);
    friend bool operator==(const DataVal& lhs, const DataVal& rhs);
    friend bool operator!=(const DataVal& lhs, const DataVal& rhs);
};

#endif
