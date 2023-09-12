#include "data/cluster_vector.h"
#include "util/util_math.h"
#include "util/util_cluster.h"

int Cluster_Vector::size()
{
    return clusters.size();
}

// modifiers
Cluster* Cluster_Vector::join(Cluster *cl1, Cluster *cl2, std::vector<std::pair<Cluster*, Cluster*>> &to_update)
{
    Cluster* joined = Util_Cluster::join(cl1, cl2, this->get_d(), false);
    clusters.push_back(joined);
    clusters.erase(cl1);
    clusters.erase(cl2);
    to_update.reserve(clusters.size());
    for(Cluster *cl : clusters) if(cl != joined) to_update.emplace_back(joined, cl);
    return joined;
}

// initialize clusters given the data
void Cluster_Vector::init_clusters_fine_grained()
{
    clusters.reserve(this->get_data().size());
    float dist = get_d();

    for(Data *d : get_data())
    {
        Cluster *cl = new Cluster(d);
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

void Cluster_Vector::clear()
{
    for(Cluster *cl : clusters) delete cl;
    clusters.clear();
}