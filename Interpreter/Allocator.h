
#include <list>
#include <utility>
#include <iostream>
#include "utils.h"

struct DataVal;

template <typename T>
struct pool {
    std::pair<size_t, T*> alloc();
    bool free(size_t listIdx);
    double percentFull() const;

    static const int POOL_SIZE = 10000;
    T poolBuf[POOL_SIZE];
    std::list<size_t> useList;
    int ctr = 0;
};


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
    static constexpr double GC_THRESHOLD = 0.8;
    
    double maxMemoryThreshold();
    void gc();

    template <typename T>
    DataVal allocCommon(int type, pool<T>& pool, T val);

    pool<double> doublePool;
    pool<int> intPool;
    pool<std::string> stringPool;
    std::unordered_map<void*, std::pair<int, DataVal> > refCounts;
};
