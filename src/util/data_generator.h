#include <random>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>

#include "data/data.h"

#ifndef __data_generator_include__
#define __data_generator_include__

class Data_Generator
{
    private:
        // random seed
        static std::random_device rd;
        static std::mt19937 gen;

        static void random_data_vector(float *arr, int dimension, int min, int max)
        {
            std::uniform_real_distribution<float> ud(min, max);
            for(int i = 0; i < dimension; i++)
            {
                arr[i] = ud(gen);
            }
        }

        static float distance(float *arr1, float *arr2, int dim)
        {
            float dist = 0.0f;

            for(int i = 0; i < dim; i++)
            {
                float diff = arr1[i]-arr2[i];
                dist += diff*diff;
            }

            return std::sqrt(dist);
        }

        static bool valid_center(float *vec, float *centers, int dim, int pos, float diameter)
        {
            for(int i = 0; i < pos; i++)
            {
                float dist = distance(vec, &centers[dim*i], dim);
                if(dist < diameter) return false;
            }

            return true;
        }
    public:
        static void generate(std::vector<Data*> &data_vec, int dimension, int n, int labels, float variance)
        {
            float *centers = (float*) malloc(sizeof(float)*dimension*labels);
            float radius = 3*variance;
            int space_limit[2] = {int(4*variance), int(labels*2*variance)};
            int elems_per_label = n/labels;

            for(int i = 0; i < labels; i++)
            {
                float center[dimension];
                random_data_vector(center, dimension, space_limit[0], space_limit[1]);
                while(!valid_center(center, centers, dimension, i, 1.5*radius))
                {
                    random_data_vector(center, dimension, space_limit[0], space_limit[1]);
                }
                
                for(int j = 0; j < dimension; j++)
                {
                    centers[dimension*i+j] = center[j];
                }

                std::normal_distribution<float> nds[dimension];
                for(int j = 0; j < dimension; j++) nds[j] = std::normal_distribution<float>(center[j], variance);

                for(int j = n*i/labels; j < n*(i+1)/labels; j++)
                {
                    Data *d = new Data(std::to_string(i));
                    d->reserve(dimension);
                    for(int k = 0; k < dimension; k++)
                    {
                        float ran = nds[k](gen);
                        d->push_back(ran);
                    }
                    data_vec.push_back(d);
                }
                
            }

            free(centers);
        }


};

#endif