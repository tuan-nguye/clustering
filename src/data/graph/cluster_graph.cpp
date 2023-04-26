#include <iostream>

#include "data/graph/cluster_graph.h"
#include "util/util.h"

Cluster_Graph::Cluster_Graph(float d): NN_Graph<Cluster*>(d, compare), d(d) {}

void Cluster_Graph::add_data(Data *d)
{
    data.push_back(d);
}

int Cluster_Graph::size()
{
    return NN_Graph::size();
}

Cluster* Cluster_Graph::join(Cluster *cl1, Cluster *cl2)
{
    if(cl1->size() < cl2->size())
    {
        Cluster *temp = cl1;
        cl1 = cl2;
        cl2 = temp;
    }

    // update graph by making union of their node children
    NN_Graph::combine_node_to_from(cl1, cl2);
    
    // update elements and values
    cl1->join(*cl2);

    return cl1;
}



void Cluster_Graph::get_neighbours(std::vector<Cluster*>& vec, Cluster *cl)
{
    NN_Graph::get_children(vec, cl);
}

void Cluster_Graph::init_clusters_fine_grained()
{
    for(Data *d : data)
    {
        Cluster *cl = new Cluster(this->d);
        cl->push_back(d);
        cl->add_to_sum(*d);
        cl->add_to_sum_of_squares(Util::sum_of_squares(*d));
        NN_Graph::add_node(cl);
    }
}

Cluster** Cluster_Graph::begin()
{
    return NN_Graph::get_all_elements().begin();
}

Cluster** Cluster_Graph::end()
{
    return NN_Graph::get_all_elements().end();
}

Cluster*& Cluster_Graph::operator[](int idx)
{
    return NN_Graph::get_all_elements()[idx];
}

float Cluster_Graph::compare(Cluster *&cl1, Cluster *&cl2)
{
    return Util::euclidean_distance(*(*cl1)[0], *(*cl2)[0]);
}