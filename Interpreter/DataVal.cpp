#include "DataVal.h"
#include "utils.h"

using namespace std;

Allocator DataVal::allocator;

DataVal::DataVal() : data(nullptr), type(D_NONE) {
}


string DataVal::toString() const {
    switch (type) {
    case D_STRING:
	return DATAVAL_GET_VAL(string, data);
    case D_INT:
	return to_string(DATAVAL_GET_VAL(int, data));
    case D_REAL:
	return to_string(DATAVAL_GET_VAL(double, data));
    default:
	break;
    }
    return "";
}

bool DataVal::toBool() const {
    switch(type) {
    case D_INT:
	return DATAVAL_GET_VAL(int, data);
    case D_REAL:
	return DATAVAL_GET_VAL(double, data) != 0;
    default:
	break;
    }
    return false;
}

bool DataVal::isNumeric() const {
    return type == D_INT || type == D_REAL;
}

std::ostream &operator<< (std::ostream &os, DataVal const &dv) {
    os << dv.toString();
    return os;
}

bool operator==(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_COMPARISON_BODY(==)
}

bool operator!=(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_COMPARISON_BODY(!=)
}

bool operator<(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_COMPARISON_BODY(<)
}

bool operator>(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_COMPARISON_BODY(>)
}

DataVal operator+(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_OPERATION_BODY(+)
}

DataVal operator-(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_NUMERIC_OPERATION_BODY(-)
}

DataVal operator*(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_NUMERIC_OPERATION_BODY(*)
}

DataVal operator/(const DataVal& lhs, const DataVal& rhs) {
    DATAVAL_NUMERIC_OPERATION_BODY(/)
}
