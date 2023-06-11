#include "data/cluster.h"
#include "util/util.h"

#ifndef __util_cluster__
#define __util_cluster__

class Util_Cluster
{
    public:
        static Cluster* join(Cluster *cl1, Cluster *cl2);
        static float f_diff(Cluster *cl1, Cluster *cl2);
        static float d_diff(Cluster *cl1, Cluster *cl2, float d);
};

#endif