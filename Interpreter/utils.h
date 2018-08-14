#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include "options.h"

class AST;

namespace utils {
    
    inline void fatalError(std::string err) {
        std::cerr << "FATAL: " << err << std::endl;
        exit(1);
    }
    
    inline int toInt(const char c) {
        if (!isdigit(c)) throw std::invalid_argument("Non-digit character found");
        return c - '0';
    }
    
    inline double toDouble(const std::string s) {
        double result = stod(s);
        return result;
    }
    
    inline void toUpper(std::string& s) {
        transform(s.begin(), s.end(), s.begin(), ::toupper);
    }
    
    inline bool inArray(const std::string &value, const std::vector<std::string> &array) {
        return std::find(array.begin(), array.end(), value) != array.end();
    }

    template <typename T>
    inline void combineArrs(std::vector<T*>* a, std::vector<T*>* b) {
        move(b->begin(), b->end(), back_inserter(*a));
    }
    
    template <typename K, typename V> class OrderedMap {
    public:
        std::vector<K> order;
        V* get(K key) const {
            if (m_map.find(key) == m_map.end()) return nullptr;
            return m_map.find(key)->second;
        }
        V* add(K key, V* value) {
            m_map[key] = value;
            order.insert(order.begin(), key);
            return value;
        }
    private:
        std::unordered_map<K, V*> m_map;
    };
}

#endif
