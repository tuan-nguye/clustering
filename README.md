# Clustering Algorithm

This is the repository for the clustering algorithm I implemented for a research project. It's written in C++ for maximum performance.

The greedy algorithm starts by giving each data element its own cluster. Using the cost function, that takes the euclidean distance between the points into account, clusters which would improve the objective value, are joined. This process repeats until there is no gain left.

## Optimizations

Executing the algorithm without any optimizations leads to long runtimes. That's why several techniques were added to improve the algorithm's time and space complexity.

### Cache
By using a cache, previous calculations can be reused, saving us a lot of calculations.

### Approximated KNN-Graph
If you want to find two clusters to join which would improve the objective value the most, you would have to iterate through every pair between them. This costs a lot of time because the amount of edges is quadratic. To reduce the effort an approximated KNN-Graph, taken from the paper "Efficient and robust approximate nearest neighbor search using Hierarchical Navigable Small World graphs" by Yu. A. Malkov and D. A. Yashunin (https://arxiv.org/abs/1603.09320), was used. The number of edges to iterate through is now linear.

### Lazy Implementation
The lazy implementation helps in updating the graph less times, instead of in every iteration. The idea is taken from the paper "ClusterFuG: Clustering Fully connected Graphs by Multicut" by Ahmed Abbas and Paul Swoboda (https://arxiv.org/abs/2301.12159).


## Results

The algorithm was tested on randomly generated data consisting of 100.000 elements with 1000 features each. It has:

- high performance
- low memory usage
- (only <i>minimal</i> memory leaks)

The execution time was only <b> 27.08 seconds </b> and it consumed a mere <b> 1.29 GB </b> of memory at its peak. The accuracy of the clustering was evaluated using the Rand Index in which it scored almost perfectly with a value of 0.999.
