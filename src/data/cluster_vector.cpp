#include "data/cluster_vector.h"

void Cluster_Vector::add_data(Data *data)
{
    Cluster cl;
    cl.add_element(data);
    clusters.push_back(cl);
}

int Cluster_Vector::join(int i, int j)
{
    if(i == j) return -1;
    else if(i > j) std::swap(i, j);

    // add all elements from cl2 to cl1
    Cluster cl1 = clusters[i], cl2 = clusters[j];
    cl1.add_all_elements(cl2.elements);

    // update sum and square_sum
    cl1.sum += cl2.sum;
    cl1.square_sum += cl2.square_sum;

    // move last cluster to position j and remove cl2
    clusters[j] = clusters.back();
    clusters.pop_back();

    return i;
}