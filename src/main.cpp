#include <iostream>
#include <vector>
#include <thread>

#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/data.h"
#include "algorithm/greedy_joining.h"
#include "eval/rand_index.h"
#include "eval/adjusted_rand_index.h"
#include "eval/silhouette_score.h"
#include "data/structures/maptor.h"
#include "data/graph/nearest_neighbour_graph.h"
#include "util/time/time.h"

void print_digit_with_label(Data *d)
{
    for(int i = 0; i < d->size(); i++)
    {
        if(i % 28 == 0) std::cout << "\n";
        if((*d)[i]) std::cout << (*d)[i];
        else std::cout << "...";
    }

    std::cout << "\nlabel: " << d->label << std::endl;
}

void print_some_info_about_distances(std::vector<Data*> &data)
{
    int count_same = 0, count_diff = 0;
    float sum_same = 0, sum_diff = 0;
    
    for(int i = 0; i < 100; i++)
    {
        Data *d1 = data[i];
        for(int j = i+1; j < 100; j++)
        {
            Data *d2 = data[j];
            float d = Util::euclidean_distance(*d1, *d2);
            std::cout << d1->label << " - " << d2->label;
            std::cout << ": eucl-dist: " << d << std::endl;

            if(d1->label == d2->label)
            {
                count_same++;
                sum_same += d;
                //print_digit_with_label(d1);
                //print_digit_with_label(d2);
            } else
            {
                count_diff++;
                sum_diff += d;
            }
        }
    }

    float avg_same = sum_same/count_same, avg_diff = sum_diff/count_diff;
    std::cout << "average same labels: " << avg_same << ", average diff labels: " << avg_diff << std::endl;
}

int num_threads = std::thread::hardware_concurrency();

int main()
{
    std::vector<Data*> data;

    Parser *parser;
    CSV_Parser csv_parser;
    Ubyte_Parser ubyte_parser;

    //parser = &csv_parser;
    //parser->parse(data, "./res/iris/iris_data.data");
    //parser->parse(data, "./res/test/test_example.data");
    parser = &ubyte_parser;
    //parser->parse(data, "./res/mnist/t10k-images.idx3-ubyte", "./res/mnist/t10k-labels.idx1-ubyte");
    parser->parse(data, "./res/mnist/train-images.idx3-ubyte", "./res/mnist/train-labels.idx1-ubyte");
    std::cout << "number of data objects: " << data.size() << std::endl;

    Time timer;

    Greedy_Joining gr_joining;
    Clustering *clustering = &gr_joining;

    timer.start();
    float d = 1500.0f; // test: 4.0, iris: 1.2, mnist: 2000.0
    std::unordered_map<Data*, std::string> clustering_result = clustering->execute(data, d);
    std::cout << "runtime in seconds: " << timer.stop() << std::endl;

    std::unordered_set<std::string> labels;
    for(auto &entry : clustering_result)
    {
        //std::cout << "cl_label: " << entry.second << ", " << entry.first->to_string() << std::endl;
        labels.insert(entry.second);
    }

    std::cout << "number of clusters: " << labels.size() << std::endl;
    
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