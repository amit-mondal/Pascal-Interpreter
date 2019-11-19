#include "DataVal.h"
#include <iostream>
#include <algorithm>

using namespace std;

Allocator::Allocator() {
    
}

template <typename T>
DataVal Allocator::allocCommon(int type, pool<T> pool, T val) {
    cout << "allocating " << val << endl;
    DataVal dval;
    auto idxWithVal = pool.alloc();
    *idxWithVal.second = val;
    dval.listIdx = idxWithVal.first;
    dval.type = (DataVal::Type) type;
    dval.data = idxWithVal.second;
    return dval;
}

DataVal Allocator::allocate(double val) {
    return allocCommon<double>(DataVal::D_REAL, doublePool, val);
}

DataVal Allocator::allocate(int val) {
    return allocCommon<int>(DataVal::D_INT, intPool, val);
}

DataVal Allocator::allocate(string val) {
    return allocCommon<string>(DataVal::D_STRING, stringPool, val);
}

void Allocator::incRefCount(DataVal val) {
    auto itr = refCounts.find(val.data);
    if (itr == refCounts.end()) {
	refCounts[val.data] = {1, val};
    } else {
	itr->second.first++;
    }
}

void Allocator::decRefCount(DataVal val) {
    auto itr = refCounts.find(val.data);
    if (itr == refCounts.end()) {
	utils::fatalError("Couldn't decrement ref count for \"" + val.toString());
    } else {
	itr->second.first--;
    }
}


double Allocator::maxMemoryThreshold() {
    return std::max(std::max(doublePool.percentFull(), intPool.percentFull()), stringPool.percentFull());
}


void Allocator::gc() {

    if (maxMemoryThreshold() < GC_THRESHOLD) {
	return;
    }
    
    for (auto itr = refCounts.cbegin(); itr != refCounts.cend(); ) {
	if (itr->second.first == 0) {
	    free(itr->second.second, false);
	    itr = refCounts.erase(itr);
	} else {
	    itr++;
	}
    }
}

void Allocator::free(DataVal val, bool removeRefCount) {
    cout << "freeing " << val << endl;

    if (removeRefCount) {
	if (refCounts.find(val.data) != refCounts.end()) {
	    refCounts.erase(val.data);
	}
    }
    
    bool res = true;
    switch(val.type) {
    case DataVal::D_STRING: res = stringPool.free(val.listIdx);
	break;
    case DataVal::D_INT: res = intPool.free(val.listIdx);
	break;
    case DataVal::D_REAL: res = doublePool.free(val.listIdx);
	break;
    default: utils::fatalError("trying to free unsupported DataVal type");
    }

    if (!res) {
	utils::fatalError("Value \"" + val.toString() + "\" could not be freed");
    }
}
