#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

#ifndef __maptor_include__
#define __maptor_include__

template<typename T> class Maptor
{
    private:
        std::vector<T> element_vec;
        std::unordered_map<T, int> idx_map;
        void merge_sort(int l, int r)
        {
            if(r-l <= 1) return;

            int m = (l+r)/2;
            merge_sort(l, m);
            merge_sort(m, r);

            std::vector<T> temp_array(element_vec.begin()+l, element_vec.begin()+r);
            int i = l, tl = 0, tm = m-l, tr = tm;
            while(i < r)
            {
                if(tl == tm)
                {
                    idx_map[temp_array[tr]] = i;
                    element_vec[i++] = temp_array[tr++];   
                } else if(tr == r-l)
                {
                    idx_map[temp_array[tl]] = i;
                    element_vec[i++] = temp_array[tl++];
                } else
                {
                    if(temp_array[tl] < temp_array[tr])
                    {
                        idx_map[temp_array[tl]] = i;
                        element_vec[i++] = temp_array[tl++];
                    } else
                    {
                        idx_map[temp_array[tr]] = i;
                        element_vec[i++] = temp_array[tr++];
                    }
                }
            }
        }
    public:
        void push_back(T &elem)
        {
            if(idx_map.find(elem) != idx_map.end()) return;
            idx_map[elem] = element_vec.size();
            element_vec.push_back(elem);
        }

        const void push_back(const T &elem)
        {
            if(idx_map.find(elem) != idx_map.end()) return;
            idx_map[elem] = element_vec.size();
            element_vec.push_back(elem);
        }
        
        void push_back(T &&elem)
        {
            if(idx_map.find(elem) != idx_map.end()) return;
            idx_map[elem] = element_vec.size();
            element_vec.push_back(elem);
        }

        bool erase(T &elem)
        {
            if(!find(elem)) return false;
            int idx = idx_map[elem];
            T &last = element_vec.back();
            element_vec[idx] = last;
            idx_map[last] = idx;
            element_vec.pop_back();
            idx_map.erase(elem);
            if(element_vec.size() != idx_map.size()) throw std::invalid_argument("maptor");
            return true;
        }

        bool erase(T &&elem)
        {
            if(!find(elem)) return false;
            int idx = idx_map[elem];
            T &last = element_vec.back();
            element_vec[idx] = last;
            idx_map[last] = idx;
            element_vec.pop_back();
            idx_map.erase(elem);
            return true;
        }

        T* erase(T *pos)
        {
            if(!erase(*pos)) return this->end();
            return pos;
        }

        int size()
        {
            return element_vec.size();
        }

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

        void clear()
        {
            element_vec.clear();
            idx_map.clear();
        }

        void sort()
        {
            merge_sort(0, size());
        }

        T* begin() { return &element_vec[0]; }
        const T* begin() const { return &element_vec[0]; }

        T* end() { return &element_vec[size()]; }
        const T* end() const { return &element_vec[size()]; }

        T& operator[] (int idx) { return element_vec[idx]; }

        std::vector<T>& get_vector() { return element_vec; }
};

#endif