#include "DataVal.h"
#include "options.h"
#include <iostream>
#include <algorithm>

using namespace std;


template <typename T>
std::pair<size_t, T*> pool<T>::alloc() {
    size_t lastIdx = 0;
    auto itr = useList.begin();
    if (itr != useList.end()) {
	while (itr != useList.end()) {
	    if (lastIdx < *itr) {
		break;
	    }
	    lastIdx = *itr + 1;
	    itr++;
	}
    }
    if (lastIdx > POOL_SIZE - 1) {
	utils::fatalError("Allocator out of memory");
    }
    useList.push_back(lastIdx);
    ctr++;	    
    return {lastIdx, &poolBuf[lastIdx]};
}

template <typename T>
bool pool<T>::free(size_t listIdx) {
    for (auto itr = useList.begin(); itr != useList.end(); itr++) {
	if (*itr == listIdx) {
	    useList.erase(itr);
	    ctr--;
	    return true;
	}
	if (*itr > listIdx) {
	    return false;
	}
    }
    return false;
}

template <typename T>
double pool<T>::percentFull() const {
    return ((double) useList.size() * 100 / POOL_SIZE);
}

Allocator::Allocator() {
    
}

template <typename T>
DataVal Allocator::allocCommon(int type, pool<T>& pool, T val) {
    if (options::showAllocations) {
	cout << "allocating, pool for " << type << " is " << pool.percentFull() << "% full" << endl;
    }
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
	refCounts.insert({val.data, {1, val}});
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
    if (options::showAllocations) {
	cout << "freeing " << val << endl;
    }

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
    case DataVal::D_NONE: utils::fatalError("trying to free empty value");
	break;
    default: utils::fatalError("trying to free unsupported DataVal type ");
    }

    if (!res) {
	utils::fatalError("Value \"" + val.toString() + "\" could not be freed");
    }
}
