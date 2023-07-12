#include "data/cluster.h"
#include "util/util_math.h"

#ifndef __util_cluster_include__
#define __util_cluster_include__

class Util_Cluster
{
    public:
        static Cluster* join(Cluster *&cl1, Cluster *&cl2, float d);
        static float f_diff(Cluster *&cl1, Cluster *&cl2);
        static float d_diff(Cluster *&cl1, Cluster *&cl2, float d);
        static float score_diff(Cluster *&cl1, Cluster *&cl2, float d);
        static float min_distance(Cluster *&cl1, Cluster *&cl2);
        static float avg_distance(Cluster *&cl1, Cluster *&cl2);
};

#endif