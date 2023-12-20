#include "data/cluster_vector.h"
#include "util/util_math.h"
#include "util/util_cluster.h"

/**
 * @brief returns the current number of clusters
 * 
 * @return int 
 */
int Cluster_Vector::size()
{
    return clusters.size();
}

//// modifiers

/**
 * @brief join two clusters by simply removing the cl1 and cl2 and adding a new one to the vector.
 * to_update contains an edge to all other clusters because it's similar to a complete graph.
 * 
 * @param cl1 cluster 1 to be joined
 * @param cl2 cluster 2 to be joined
 * @param to_update vector that stores all edges whose values need to be updated 
 * @return Cluster* 
 */
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

//// access functions

/**
 * @brief get all neighbours to the cluster cl and store them in vec. Similar to a complete
 * graph a cluster has all others as its neighbours.
 * 
 * @param vec 
 * @param cl 
 */
void Cluster_Vector::get_neighbours(std::vector<Cluster*>& vec, Cluster *cl)
{
    for(Cluster* nb : clusters)
    {
        if(nb == cl) continue;
        vec.push_back(nb);
    }
}

/**
 * @brief check whether a cluster is contained
 * 
 * @param cl 
 * @return true 
 * @return false 
 */
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