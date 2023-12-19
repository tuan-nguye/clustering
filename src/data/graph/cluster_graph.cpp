#include <iostream>

#include "data/graph/cluster_graph.h"
#include "util/util_math.h"
#include "util/util_cluster.h"

Cluster_Graph::Cluster_Graph(float d, Auto_Edge_Graph<Cluster*> *ae_graph): Cluster_Container(d), ae_graph(ae_graph) {}

int Cluster_Graph::size() { return ae_graph->size(); }

/**
 * @brief create new cluster which is the union of cl1 and cl2 and
 * update graph by making union of their node children
 * delete old clusters from heap
 * 
 * @param cl1 
 * @param cl2 
 * @param to_update 
 * @return Cluster* 
 */
Cluster* Cluster_Graph::join(Cluster *cl1, Cluster *cl2, std::vector<std::pair<Cluster*, Cluster*>> &to_update)
{
    Cluster *combined = Util_Cluster::join(cl1, cl2, this->get_d(), false);
    ae_graph->combine_nodes_into(combined, cl1, cl2, to_update);
    return combined;
}

// access functions

/**
 * @brief get all of the neighbours/children of the cluster cl
 * 
 * @param vec children are stored in this vector
 * @param cl 
 */
void Cluster_Graph::get_neighbours(std::vector<Cluster*>& vec, Cluster *cl)
{
    ae_graph->get_neighbours(vec, cl);
}

/**
 * @brief returns true if the cluster is contained in the graph
 * 
 * @param cl 
 * @return true 
 * @return false 
 */
bool Cluster_Graph::find(Cluster *&cl)
{
    return ae_graph->find_node(cl);
}

/**
 * @brief initialize all clusters by giving each element its own cluster
 * 
 */
void Cluster_Graph::init_clusters_fine_grained()
{
    float dist = get_d();
    std::vector<Cluster*> clusters;
    clusters.reserve(get_data().size());

    for(Data *d : get_data())
    {
        Cluster *cl = new Cluster(d);
        clusters.push_back(cl);
    }

    ae_graph->set_nodes(clusters);
}

/**
 * @brief rebuild the entire graph by adding edges
 * 
 * @param to_update 
 */
void Cluster_Graph::rebuild(std::vector<std::pair<Cluster*, Cluster*>> &to_update)
{
    ae_graph->rebuild(to_update);
}

/**
 * @brief returns a pointer to the first cluster
 * 
 * @return Cluster** 
 */
Cluster** Cluster_Graph::begin()
{
    return ae_graph->get_all_elements().begin();
}

/**
 * @brief returns a pointer to the place after the last element
 * 
 * @return Cluster** 
 */
Cluster** Cluster_Graph::end()
{
    return ae_graph->get_all_elements().end();
}

/**
 * @brief access operator for the clusters
 * 
 * @param idx 
 * @return Cluster*& 
 */
Cluster*& Cluster_Graph::operator[](int idx)
{
    return ae_graph->get_all_elements()[idx];
}

/**
 * @brief clear all clusters and free their memory
 * 
 */
void Cluster_Graph::clear()
{
    for(Cluster *cl : ae_graph->get_all_elements()) delete cl;
    ae_graph->clear();
}