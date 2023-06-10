#include "data/cluster_vector.h"

int Cluster_Vector::size()
{
    return clusters.size();
}

// modifiers
Cluster* Cluster_Vector::join(Cluster *cl1, Cluster *cl2)
{
    Cluster* joined = &Cluster::join(cl1, cl2);
    cl1->clear();
    cl2->clear();
    clusters.erase(cl1);
    clusters.erase(cl2);
    return joined;
}

// initialize clusters given the data
void Cluster_Vector::init_clusters_fine_grained()
{
    float dist = get_d();

    for(Data *d : get_data())
    {
        Cluster *cl = new Cluster(dist);
        cl->push_back(d);
        cl->add_to_sum(*d);
        cl->add_to_sum_of_squares(Util::sum_of_squares(*d));
        clusters.push_back(cl);
    }
}

// access functions
void Cluster_Vector::get_neighbours(std::vector<Cluster*>& vec, Cluster *cl)
{

}

bool Cluster_Vector::find(Cluster *&cl)
{
    return clusters.find(cl);
}

Cluster** Cluster_Vector::begin()
{
    return clusters.begin();
}

Cluster** Cluster_Vector::end()
{
    return clusters.end();
}

Cluster*& Cluster_Vector::operator[](int idx)
{
    return clusters[idx];
}