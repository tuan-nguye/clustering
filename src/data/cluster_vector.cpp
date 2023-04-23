#include "data/cluster_vector.h"

Cluster* Cluster_Vector::add_empty_cluster()
{
    Cluster *cl = new Cluster();
    push_back(cl);
    return cl;
}

int Cluster_Vector::join(int i, int j)
{
    if(i == j) return -1;
    else if(i > j) std::swap(i, j);

    // add all elements from cl2 to cl1
    Cluster *cl1 = (*this)[i], *cl2 = (*this)[j];
    cl1->insert(cl1->end(), cl2->begin(), cl2->end());
    
    // update sum and square_sum
    for(int i = 0; i < cl1->get_sum().size(); i++) cl1->get_sum()[i] += cl2->get_sum()[i];
    cl1->sum_of_squares += cl2->sum_of_squares;

    // delete cl2, move last cluster to position j and remove old position
    (*this)[j] = back();
    pop_back();
    delete cl2;

    return i;
}