#include <vector>
#include <unordered_map>

#ifndef __maptor_include__
#define __maptor_include__

template<typename T> class Maptor
{
    private:
        std::vector<T> element_vec;
        std::unordered_map<T> idx_map;
    public:
        void push_back(T &elem)
        {
            if(idx_map.find(elem) != idx_map.end()) return;
            idx_map[elem] = element_vec.size();
            element_vec.push_back(elem);
        }

        T& erase(T &elem)
        {
            if(!find(elem)) return NULL;
            int idx = idx_map[elem];
            T &last = element_vec.back();
            element_vec[idx] = last;
            idx_map[last] = idx;

            element_vec.pop_back();
            idx_map.erase(elem);
        }

        int size() { return element_vec.size(); }

        bool empty() { return element_vec.empty(); }

        void reserve(size_t size)
        {
            element_vec.reserve(size);
            idx_map.reserve(size);
        }

        bool find(T &elem)
        {
            return idx_map.find(elem) != idx_map.end();
        }

        T* begin() { return &element_vec[0]; }
        const T* begin() const { return &element_vec[0]; }

        T* end() { return &element_vec[size()]; }
        const T* end() const { return &element_vec[size()]; }
};

#endif