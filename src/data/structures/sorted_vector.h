#include <vector>
#include <functional>

#ifndef __sorted_vector_include__
#define __sorted_vector_include__

template<typename T> class Sorted_Vector
{
    private:
        std::vector<T> vector;
        std::function<float(T&, T&)> cmp;

        int binary_search(T &t)
        {
            if(vector.empty()) return 0;
            int l = 0, r = vector.size()-1;

            while(l <= r)
            {
                int m = (l+r)/2;
                //std::cout << "(l, m, r, size) = " << "(" << l << ", " << m << ", " << r << ", " << vector.size() << ")" << std::endl;
                T &tm = vector[m];
                if(tm == t) return m;

                float diff = cmp(tm, t);
                if(diff == 0)
                {
                    int idx_lin = linear_search(t);
                    return idx_lin != vector.size() ? idx_lin : m;
                } else if(diff < 0)
                {
                    l = m+1;
                } else
                {
                    r = m-1;
                }
            }
            
            return l;
        }

        int linear_search(T &t)
        {
            int idx = 0;

            while(idx < vector.size())
            {
                if(vector[idx] == t) break;
                idx++;
            }

            return idx;
        }

    public:
        Sorted_Vector(std::function<float(T&, T&)> &func): cmp(func) {}

        void push(T &t)
        {
            int idx = binary_search(t);
            if(idx == vector.size()) vector.push_back(t);
            else vector.insert(vector.begin()+idx, t);
        }

        void erase(T &t)
        {
            int idx = linear_search(t);
            if(idx == vector.size() || vector[idx] != t) return;
            vector.erase(vector.begin()+idx);
        }

        void erase(T &&t)
        {
            int idx = linear_search(t);
            if(idx == vector.size() || vector[idx] != t) return;
            vector.erase(vector.begin()+idx);
        }

        T& back()
        {
            return vector.back();
        }

        int size()
        {
            return vector.size();
        }

        typename std::vector<T>::iterator begin() { return vector.begin(); }
        typename std::vector<T>::iterator end() { return vector.end(); }
};

#endif