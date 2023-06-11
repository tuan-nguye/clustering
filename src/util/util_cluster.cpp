#include "util/util_cluster.h"

Cluster* Util_Cluster::join(Cluster *cl1, Cluster *cl2)
{
    Cluster* cl_joined = new Cluster(cl1->get_distance());
    cl_joined->set_f(cl1->get_f() + cl2->get_f() + cl1->size()*cl2->get_sum_of_squares() + cl2->size()*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum()));
    cl_joined->set_score(cl_joined->get_f()-Util::d_all_pairs(cl1->size()+cl2->size(), cl_joined->get_distance()));

    cl_joined->add_to_sum(cl1->get_sum());
    cl_joined->add_to_sum(cl2->get_sum());

    cl_joined->add_to_sum_of_squares(cl1->get_sum_of_squares());
    cl_joined->add_to_sum_of_squares(cl2->get_sum_of_squares());

    cl_joined->insert(cl_joined->end(), cl1->begin(), cl1->end());
    cl_joined->insert(cl_joined->end(), cl2->begin(), cl2->end());

    return cl_joined;
}

float Util_Cluster::f_diff(Cluster *cl1, Cluster *cl2)
{
    return cl1->size()*cl2->get_sum_of_squares() + cl2->size()*cl1->get_sum_of_squares() - 2*Util::scalar_product(cl1->get_sum(), cl2->get_sum());
}

float Util_Cluster::d_diff(Cluster *cl1, Cluster *cl2, float d)
{
    float cl1_size = cl1->size(), cl2_size = cl2->size();
    return Util::d_all_pairs(cl1_size, d) + Util::d_all_pairs(cl2_size, d) - Util::d_all_pairs(cl1_size+cl2_size, d);
}