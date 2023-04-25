#include <iostream>
#include <vector>
#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/data.h"
#include "algorithm/greedy_joining.h"
#include "eval/rand_index.h"
#include "eval/adjusted_rand_index.h"
#include "eval/silhouette_score.h"
#include "data/structures/maptor.h"
#include "data/graph/nearest_neighbour_graph.h"

float compare(Data *&d1, Data *&d2)
{
    return Util::euclidean_distance(*d1, *d2);
}

int main()
{
    Parser *parser;
    std::vector<Data*> data;

    // csv test
    CSV_Parser csv_parser;
    Ubyte_Parser ubyte_parser;

    parser = &csv_parser;
    parser->parse(data, "./res/iris/iris_data.data");
    //parser->parse(data, "./res/test/test_example.data");
    //parser = &ubyte_parser;
    //parser->parse(data, "./res/mnist/t10k-images.idx3-ubyte", "./res/mnist/t10k-labels.idx1-ubyte");
    std::cout << "number of data objects: " << data.size() << std::endl;

    /*for(int i = 0; i < 9; i++)
    {
        Data *d1 = data[i];
        for(int j = i+1; j < 10; j++)
        {
            Data *d2 = data[j];
            std::cout << d1->label << " - " << d2->label << "\n";
            std::cout << "eucl-dist: " << Util::euclidean_distance(*d1, *d2) << std::endl;
        }
    }*/
    /*float (*cmp)(Data*&, Data*&);
    cmp = &compare;
    NN_Graph<Data*> nn_graph(4.0f, compare);
    for(Data *&d : data) nn_graph.add_node(d);

    nn_graph.remove_node(data[4]);
    nn_graph.remove_node(data[0]);

    for(Data *&d : nn_graph.get_all_values())
    {
        std::cout << d->to_string() << ", children: ";
        std::vector<Data*> children;
        nn_graph.get_children(children, d);

        for(Data *c : children)
        {
            std::cout << c->to_string() << ", ";
        }

        std::cout << std::endl;
    }

    return 0;*/
    Greedy_Joining gr_joining;
    Clustering *clustering = &gr_joining;

    float d = 1.2f; // test: 4.0, iris: 1.2, mnist: 1500.0
    std::unordered_map<Data*, std::string> clustering_result = clustering->execute(data, d);
    
    for(auto &entry : clustering_result)
    {
        std::cout << "cl_label: " << entry.second << ", " << entry.first->to_string() << std::endl;
    }
    
    Rand_Index ri = Rand_Index();
    Adjusted_Rand_Index ari = Adjusted_Rand_Index();
    Silhouette_Score ss = Silhouette_Score();
    std::vector<Evaluation*> eval_list = {&ri, &ari, &ss};

    for(auto &metric : eval_list)
    {
        std::cout << typeid(*metric).name() << ": " << metric->execute(clustering_result) << std::endl;
    }

    
    return 0;
}