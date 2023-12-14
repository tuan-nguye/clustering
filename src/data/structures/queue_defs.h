#include <tuple>
#include <queue>

#include "data/cluster.h"

#ifndef __queue_defs_include__
#define __queue_defs_include__

// definitions for some structures

// pair of clusters
typedef std::tuple<float, Cluster*, Cluster*> Edge;
// min priority queue for edges using a vector
typedef std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> MinPriorityQueue;
// max priority queue for edges using a vector
typedef std::priority_queue<Edge, std::vector<Edge>> MaxPriorityQueue;

#endif