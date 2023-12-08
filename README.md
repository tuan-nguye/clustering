# Clustering Algorithm

This is the repository for the clustering algorithm I implemented for a research project. It's written in C++ for maximum performance.

The greedy algorithm starts by giving each data element its own cluster. Using the cost function, that takes the euclidean distance between the points into account, clusters which would improve the objective value, are joined. This process repeats until there is no gain left.

## Results

The algorithm was tested on randomly generated data consisting of 100.000 elements with 1000 features each. Besides the high performance it also has a low memory usage (with only minimal memory leaks). The execution time was only <b> 27.08 seconds </b> and it consumed a mere <b> 1.29 GB </b> of memory at its peak. The accuracy of the clustering was evaluated using the Rand Index in which it scored almost perfectly with a value of 0.999.
