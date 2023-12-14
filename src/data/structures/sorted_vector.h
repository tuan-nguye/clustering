#include <vector>
#include <functional>

#ifndef __sorted_vector_include__
#define __sorted_vector_include__


/**
 * @brief flat sorted vector
 * it uses binary search for inserting elements
 * but linear search for deletion because evaluating cmp takes a lot of time (due to clustering cost implementation)
 * so that a simple check for equality is faster. The performance is pretty good if the size of the vector is limited.
 * @tparam T 
 */
template<typename T> class Sorted_Vector
{
    private:
        std::vector<T> vector;
        std::function<float(T&, T&)> cmp;

        /**
         * @brief find the index of an element by using binary search
         * 
         * @param t element
         * @return int: the index where the element is stored or the first element that
         * is larger than t if it doesn't exist 
         */
        int binary_search(T &t)
        {
            if(vector.empty()) return 0;
            int l = 0, r = vector.size()-1;

            while(l <= r)
            {
                int m = (l+r)/2;
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

        /**
         * @brief search for an element by simply iterating through the vector
         * 
         * @param t 
         * @return int 
         */
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
        Sorted_Vector(std::function<float(T&, T&)> func): cmp(func) {}

        void push(T &t)
        {
            int idx = binary_search(t);
            if(idx == vector.size()) vector.push_back(t);
            else vector.insert(vector.begin()+idx, t);
        }

        void pop_back()
        {
            vector.pop_back();
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

        bool contains(T &t)
        {
            return linear_search(t) != size();
        }
        
        bool contains(T &&t)
        {
            return linear_search(t) != size();
        }

        T& front()
        {
            return vector.fron();
        }

        T& back()
        {
            return vector.back();
        }

        int size()
        {
            return vector.size();
        }

        void clear()
        {
            vector.clear();
        }

        std::vector<T>& get_vector() { return vector; }

        typename std::vector<T>::iterator begin() { return vector.begin(); }
        typename std::vector<T>::iterator end() { return vector.end(); }
};

#endif