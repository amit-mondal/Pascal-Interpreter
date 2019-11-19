
#include <list>
#include <utility>
#include <iostream>
#include "utils.h"

struct DataVal;

class Allocator {
public:
    Allocator();
    DataVal allocate(double val);
    DataVal allocate(int val);
    DataVal allocate(std::string val);
    void incRefCount(DataVal val);
    void decRefCount(DataVal val);
    void free(DataVal val, bool removeRefCount=true);
private:
    static const int POOL_SIZE = 10000;
    static constexpr double GC_THRESHOLD = 0.8;

    template <typename T>
    struct pool {
	T poolBuf[POOL_SIZE];
	std::list<size_t> useList;
	
	std::pair<size_t, T*> alloc() {
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
	    return {lastIdx, &poolBuf[lastIdx]};
	}

	bool free(size_t listIdx) {
	    for (auto itr = useList.begin(); itr != useList.end(); itr++) {
		if (*itr == listIdx) {
		    useList.erase(itr);
		    return true;
		}
		if (*itr > listIdx) {
		    return false;
		}
	    }
	    return false;
	}

	double percentFull() const {
	    return (useList.size() / POOL_SIZE);
	}
    };
    
    double maxMemoryThreshold();
    void gc();

    template <typename T>
    DataVal allocCommon(int type, pool<T> pool, T val);

    pool<double> doublePool;
    pool<int> intPool;
    pool<std::string> stringPool;
    std::unordered_map<void*, std::pair<int, DataVal> > refCounts;
};
