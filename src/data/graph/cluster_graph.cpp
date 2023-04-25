#include <iostream>

#include "data/graph/cluster_graph.h"
#include "util/util.h"


Cluster_Graph::Cluster_Graph(std::vector<Data*> data, float d): d(d)
{
    int n = data.size();
    for(Data *dp : data)
    {
        Cluster *cl = add_empty_cluster();
        cls_idx[cl] = size()-1;
        cl->push_back(dp);
        for(float f : *dp) cl->get_sum().push_back(f);
        cl->add_to_sum_of_squares(Util::sum_of_squares(*dp));

        // add to graph
        graph.add_node(cl);
    }


    for(int i = 0; i < n-1; i++)
    {
        Cluster *cl1 = (*this)[i];
        for(int j = i+1; j < n; j++)
        {
            Cluster *cl2 = (*this)[j];
            float eucl_dist = Util::euclidean_distance(*(*cl1)[0], *(*cl2)[0]);

            if(eucl_dist <= d)
            {
                graph.add_edge(cl1, cl2);
            }
        }
    }
}

int Cluster_Graph::join(int i, int j)
{
    Cluster *cl1 = (*this)[i], *cl2 = (*this)[j];
    std::vector<Cluster*> children2;
    graph.get_children(children2, cl2);

    for(Cluster *next2 : children2)
    {
        if(cl1 == next2) continue;
        graph.add_edge(cl1, next2);
    }

    // update index map
    Cluster *end = back();
    cls_idx[end] = cls_idx[cl2];
    cls_idx.erase(cl2);

    graph.remove_node(cl2);
    return Cluster_Vector::join(i, j);
}

void Cluster_Graph::get_neighbours(std::vector<Cluster*>& vec, Cluster *cl)
{
    graph.get_children(vec, cl);
}

int Cluster_Graph::get_cluster_index(Cluster *cl)
{
    return cls_idx[cl];
}