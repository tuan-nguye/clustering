#include <tuple>
#include <queue>

#include "data/cluster.h"

#ifndef __queue_defs_include__
#define __queue_defs_include__

typedef std::tuple<float, Cluster*, Cluster*> Edge;
typedef std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> MinPriorityQueue;
typedef std::priority_queue<Edge, std::vector<Edge>> MaxPriorityQueue;

#endif