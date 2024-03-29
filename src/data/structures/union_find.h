#include <vector>
#include <unordered_map>
#include <tuple>

#ifndef __union_find_include__
#define __union_find_include__

/**
 * @brief union-find or disjoint-set data structure
 * The tree is balanced using the depth of the trees. The rank helps keeping the depth
 * of the resulting tree to be as small as possible
 * @tparam T 
 */
template <typename T> class Union_Find
{
    private:
        typedef std::tuple<T, int, int> el_tuple;
        std::vector<el_tuple> elem; // tuple of (T element, int parent_index, int height_rank)
        std::unordered_map<T, int> idx_map;
    public:
        Union_Find() {}
    
        Union_Find(int init_size)
        {
            reserve(init_size);
        }

        void reserve(int size)
        {
            idx_map.reserve(size);
            elem.reserve(size);
        }

        void insert(T &t)
        {
            int last = size();
            idx_map[t] = last;
            elem.emplace_back(t, last, 0);
        }

        void insert(T &&t)
        {
            int last = size();
            idx_map[t] = last;
            elem.emplace_back(t, last, 0);
        }

        // left_is_parent makes t1 the parent of t2
        // otherwise per default evaluate by rank
        void union_(T &t1, T &t2)
        {
            T &p1 = find_(t1), &p2 = find_(t2);
            if(p1 == p2) return;
            int i1 = idx_map[p1], i2 = idx_map[p2];
            el_tuple &tpl1 = elem[i1], &tpl2 = elem[i2];

            int height_diff = std::get<2>(tpl1) - std::get<2>(tpl2);
            if(height_diff == 0)
            {
                std::get<1>(tpl2) = i1;
                std::get<2>(tpl1)++;
            } else if(height_diff < 0)
            {
                std::get<1>(tpl1) = i2;
            } else
            {
                std::get<1>(tpl2) = i1;
            }
        }

        void union_(T &&t1, T &&t2)
        {
            T &p1 = find_(t1), &p2 = find_(t2);
            if(p1 == p2) return;
            int i1 = idx_map[p1], i2 = idx_map[p2];
            el_tuple &tpl1 = elem[i1], &tpl2 = elem[i2];

            int height_diff = std::get<2>(tpl1) - std::get<2>(tpl2);
            if(height_diff == 0)
            {
                std::get<1>(tpl2) = i1;
                std::get<2>(tpl1)++;
            } else if(height_diff < 0)
            {
                std::get<1>(tpl1) = i2;
            } else
            {
                std::get<1>(tpl2) = i1;
            }
        }

        T& find_(T &t)
        {
            int i = idx_map[t];
            int parent;
            while((parent = std::get<1>(elem[i])) != i) i = parent;
            return std::get<0>(elem[i]);
        }

        T& find_(T &&t)
        {
            int i = idx_map[t];
            int parent;
            while((parent = std::get<1>(elem[i])) != i) i = parent;
            return std::get<0>(elem[i]);
        }

        int size()
        {
            return elem.size();
        }

        class iterator
        {
            private:
                typedef typename std::vector<el_tuple>::iterator tpl_it;
                tpl_it it;
            public:
                iterator(tpl_it it): it(it) {}

                iterator& operator++()
                {
                    ++it;
                    return *this;
                }

                T& operator*() { return std::get<0>(*it); }

                bool operator!=(const iterator &other) { return it != other.it; }
        };

        int max_height()
        {
            int max = 0;
            for(el_tuple &tpl : elem)
            {
                if(std::get<2>(tpl) > max) max = std::get<2>(tpl);
            }
            return max;
        }

        iterator begin() { return iterator(elem.begin()); }
        iterator end() { return iterator(elem.end()); }
};

#endif