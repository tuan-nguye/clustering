#include <iostream>

#include "data/graph/cluster_graph.h"
#include "util/util.h"

Cluster_Graph::Cluster_Graph(float d): Cluster_Container(d), Distance_Graph<Cluster*>(d, compare) {}

/*
create new cluster which is the union of cl1 and cl2 and
update graph by making union of their node children
delete old clusters from heap
*/
Cluster* Cluster_Graph::join(Cluster *cl1, Cluster *cl2)
{
    Cluster *combined = &Cluster::join(cl1, cl2);
    Distance_Graph::combine_nodes_into(combined, cl1, cl2);
    cl1->clear();//delete cl1;
    cl2->clear();//delete cl2;
    return combined;
}

// access

void Cluster_Graph::get_neighbours(std::vector<Cluster*>& vec, Cluster *cl)
{
    Distance_Graph::get_children(vec, cl);
}

bool Cluster_Graph::find(Cluster *&cl)
{
    return Distance_Graph::find(cl);
}

void Cluster_Graph::init_clusters_fine_grained()
{
    float dist = get_d();

    for(Data *d : get_data())
    {
        Cluster *cl = new Cluster(dist);
        cl->push_back(d);
        cl->add_to_sum(*d);
        cl->add_to_sum_of_squares(Util::sum_of_squares(*d));
        Distance_Graph::add_node(cl);
    }
}

Cluster** Cluster_Graph::begin()
{
    return Distance_Graph::get_all_elements().begin();
}

Cluster** Cluster_Graph::end()
{
    return Distance_Graph::get_all_elements().end();
}

Cluster*& Cluster_Graph::operator[](int idx)
{
    return Distance_Graph::get_all_elements()[idx];
}

float Cluster_Graph::compare(Cluster *&cl1, Cluster *&cl2)
{
    return Util::euclidean_distance(*(*cl1)[0], *(*cl2)[0]);
}