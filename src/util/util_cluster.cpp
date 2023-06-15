#include "util/util_cluster.h"

Cluster* Util_Cluster::join(Cluster *&cl1, Cluster *&cl2, float d)
{
    Cluster* cl_joined = new Cluster();
    cl_joined->set_f(cl1->get_f() + cl2->get_f() + cl1->size()*cl2->get_sum_of_squares() + cl2->size()*cl1->get_sum_of_squares() - 2*Util_Math::scalar_product(cl1->get_sum(), cl2->get_sum()));
    cl_joined->set_score(cl_joined->get_f()-Util_Math::d_all_pairs(cl1->size()+cl2->size(), d));

    cl_joined->add_to_sum(cl1->get_sum());
    cl_joined->add_to_sum(cl2->get_sum());

    cl_joined->add_to_sum_of_squares(cl1->get_sum_of_squares());
    cl_joined->add_to_sum_of_squares(cl2->get_sum_of_squares());

    cl_joined->insert(cl_joined->end(), cl1->begin(), cl1->end());
    cl_joined->insert(cl_joined->end(), cl2->begin(), cl2->end());

    return cl_joined;
}

float Util_Cluster::f_diff(Cluster *&cl1, Cluster *&cl2)
{
    return cl1->size()*cl2->get_sum_of_squares() + cl2->size()*cl1->get_sum_of_squares() - 2*Util_Math::scalar_product(cl1->get_sum(), cl2->get_sum());
}

float Util_Cluster::d_diff(Cluster *&cl1, Cluster *&cl2, float d)
{
    float cl1_size = cl1->size(), cl2_size = cl2->size();
    return Util_Math::d_all_pairs(cl1_size, d) + Util_Math::d_all_pairs(cl2_size, d) - Util_Math::d_all_pairs(cl1_size+cl2_size, d);
}

float Util_Cluster::score_diff(Cluster *&cl1, Cluster *&cl2, float d)
{
    return f_diff(cl1, cl2) + d_diff(cl1, cl2, d);
}

float Util_Cluster::min_distance(Cluster *&cl1, Cluster *&cl2)
{
    float min = 1e9;

    for(Data *d1 : *cl1)
    {
        for(Data *d2 : *cl2)
        {
            float distance = Util_Math::euclidean_distance(*d1, *d2);
            if(distance < min) min = distance;
        }
    }

    return min;
}