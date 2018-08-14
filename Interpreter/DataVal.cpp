#include "DataVal.h"
#include "utils.h"

using namespace std;

void DataVal::freeData() {
    switch(type) {
    case D_STRING: delete (string*) data;
	break;
    case D_INT: delete (int*) data;
	break;
    case D_REAL: delete (float*) data;
	break;
    }
}

DataVal::DataVal() {
    this->type = D_NONE;
    data = nullptr;
}
    
DataVal::DataVal(double val) {
    this->type = D_REAL;
    double* newDouble = new double(val);
    data = (void*) newDouble;
}

DataVal::DataVal(int val) {
    this->type = D_INT;
    int* newInt = new int(val);
    data = (void*) newInt;
}

DataVal::DataVal(string val) {
    this->type = D_STRING;
    string* newStr = new string(val);
    data = (void*) newStr;
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
