#include "data/cluster_vector.h"
#include "util/util.h"
#include "util/util_cluster.h"

int Cluster_Vector::size()
{
    return clusters.size();
}

// modifiers
Cluster* Cluster_Vector::join(Cluster *cl1, Cluster *cl2)
{
    Cluster* joined = Util_Cluster::join(cl1, cl2);
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
    for(Cluster* nb : clusters)
    {
        if(nb == cl) continue;
        vec.push_back(nb);
    }
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