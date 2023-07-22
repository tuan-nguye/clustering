#include <iostream>

#include "data/graph/cluster_graph.h"
#include "util/util_math.h"
#include "util/util_cluster.h"

Cluster_Graph::Cluster_Graph(float d, Auto_Edge_Graph<Cluster*> *ae_graph): Cluster_Container(d), ae_graph(ae_graph) {}

int Cluster_Graph::size() { return ae_graph->size(); }

/*
create new cluster which is the union of cl1 and cl2 and
update graph by making union of their node children
delete old clusters from heap
*/
Cluster* Cluster_Graph::join(Cluster *cl1, Cluster *cl2, std::vector<std::pair<Cluster*, Cluster*>> &to_update)
{
    Cluster *combined = Util_Cluster::join(cl1, cl2, this->get_d(), false);
    ae_graph->combine_nodes_into(combined, cl1, cl2, to_update);
    return combined;
}

// access

void Cluster_Graph::get_neighbours(std::vector<Cluster*>& vec, Cluster *cl)
{
    ae_graph->get_neighbours(vec, cl);
}

bool Cluster_Graph::find(Cluster *&cl)
{
    return ae_graph->find_node(cl);
}

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

void Cluster_Graph::rebuild(std::vector<std::pair<Cluster*, Cluster*>> &to_update)
{
    ae_graph->rebuild(to_update);
}

void Cluster_Graph::delete_clusters()
{
    for(Cluster *cl : ae_graph->get_all_elements())
    {
        delete cl;
    }
    ae_graph->clear();
}

Cluster** Cluster_Graph::begin()
{
    return ae_graph->get_all_elements().begin();
}

Cluster** Cluster_Graph::end()
{
    return ae_graph->get_all_elements().end();
}

Cluster*& Cluster_Graph::operator[](int idx)
{
    return ae_graph->get_all_elements()[idx];
}