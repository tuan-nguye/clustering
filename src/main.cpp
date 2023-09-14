#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <utility>
#include <unordered_map>

#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/data.h"
#include "algorithm/greedy_joining.h"
#include "eval/rand_index.h"
#include "eval/adjusted_rand_index.h"
#include "eval/silhouette_score.h"
#include "data/structures/maptor.h"
#include "data/graph/distance_graph.h"
#include "util/time/time.h"
#include "eval/print_result_table.h"
#include "util/util_cluster.h"
#include "data/graph/auto_edge_graph.h"
#include "data/graph/knn_graph.h"
#include "data/cluster_vector.h"
#include "data/structures/sorted_vector.h"
#include "data/graph/lazy_ann_graph.h"
#include "data/graph/lazy_ann_graph2.h"
#include "eval/variation_of_information.h"
#include "data/graph/default_node.h"
#include "data/structures/union_find.h"
#include "algorithm/gaec.h"
#include "util/mem_usage.h"
#include "util/data_generator.h"

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
            float d = Util_Math::euclidean_distance(*d1, *d2);
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

void show_results(std::unordered_map<Data*, std::string> result, std::string additional_info, bool table, bool write)
{
    // print results
    Print_Result_Table print_result;
    std::string out = additional_info;

    // get evaluation indexes
    Rand_Index ri = Rand_Index();
    Adjusted_Rand_Index ari = Adjusted_Rand_Index();
    Variation_Of_Information voi = Variation_Of_Information();
    std::vector<Evaluation*> eval_list = {&ri, &ari, &voi};

    for(auto &metric : eval_list)
    {
        out += std::string(typeid(*metric).name()) + ": " + std::to_string(metric->execute(result)) + "\n";
    }

    if(table) out += "\n" + print_result.print(result);
    std::cout << out << std::endl;

    if(!write) return;
    //write result to file

    std::string root = "gen";
    if (!std::filesystem::is_directory(root))
    {
        if(!std::filesystem::create_directories(root))
        {
            std::cout << "failed to create gen/ directory" << std::endl;
        }
    }
    
    print_result.write_to_file(out, root + "/" + "mnist_60k_table.txt");
}

void repeat_and_write_csv(std::vector<Data*> &data, Clustering *clustering, float d_start, float d_end, float d_step)
{
    Print_Result_Table print_result;
    std::string out = "d,obj_value,rand_index,recall_joins,recall_cuts,adj_rand_index,variation_of_information,memory_consumption\n";

    Rand_Index ri = Rand_Index();
    Adjusted_Rand_Index ari = Adjusted_Rand_Index();
    Variation_Of_Information voi = Variation_Of_Information();
    std::vector<Evaluation*> eval_list = {&ri, &ari, &voi};

    for(float d = d_start; d <= d_end; d += d_step)
    {
        auto cmp = [d](Cluster *&cl1, Cluster *&cl2) -> float
        {
            return Util_Cluster::score_diff(cl1, cl2, d);
        };

        std::unordered_map<Data*, std::string> result = clustering->execute(data, d);

        std::cout << "clustering for d = " << d << std::endl;

        out += std::to_string(d) + ",";
        out += std::to_string(clustering->get_objective_value()) + ",";
        for(Evaluation *metric : eval_list)
        {
            out += std::to_string(metric->execute(result));
            out += ",";
            if(metric == &ri)
            {
                out += std::to_string(ri.get_recall_joins());
                out += ",";
                out += std::to_string(ri.get_recall_cuts());
                out += ",";
            }
        }
        
        //out += std::to_string(Memory_Usage::get_in_mb());
        out += "\n";
    }

    // print result
    std::cout << out << std::endl;

    // write to file

    std::string root = "gen";
    if (!std::filesystem::is_directory(root))
    {
        if(!std::filesystem::create_directories(root))
        {
            std::cout << "failed to create gen/ directory" << std::endl;
        }
    }

    print_result.write_to_file(out, root + "/" + "iris.csv");
}

// global variables
int num_threads = std::thread::hardware_concurrency();

int test()
{
    Data_Generator d_gen;
    std::vector<Data*> test;
    d_gen.generate(test, 2, 20000, 10, 15.0f);
    std::string str;
    for(Data *d : test)
    {
        str += d->to_string() + "\n";
    }
    Print_Result_Table::write_to_file(str, "gen/random_data.txt");
    return 0;
}

int main()
{
    // parse data, choose between different inputs
    std::vector<Data*> data;

    Parser *parser;
    CSV_Parser csv_parser;
    Ubyte_Parser ubyte_parser;

    //parser = &csv_parser;
    //parser->parse(data, "./res/test/test_example.data");
    //parser->parse(data, "./res/iris/iris_data.data");
    parser = &ubyte_parser;
    parser->parse(data, "./res/mnist/t10k-images.idx3-ubyte", "./res/mnist/t10k-labels.idx1-ubyte");
    //parser->parse(data, "./res/mnist/train-images.idx3-ubyte", "./res/mnist/train-labels.idx1-ubyte");
    
    // resize the data if needed and print number of data objects
    int data_size = 2000;
    data_size = data_size > data.size() ? data.size() : data_size;
    for(int i = data_size; i < data.size(); i++) delete data[i];
    data.resize(data_size);
    std::cout << "number of data objects: " << data.size() << std::endl;

    // configure algorithm parameters and select cluster data structure
    float d = 2100.0; // test: 4.0 => idx: 1, iris: 1.2 => rand_idx: 0.829799, mnist: 2200.0
    int k = 5;
    bool enable_parallel = true;
    bool enable_cache = true;
    Time timer;

    // choose underlying data structure
    Auto_Edge_Graph<Cluster*> *ae_graph;
    Distance_Graph<Cluster*> dist_graph(d, &Util_Cluster::min_distance);
    KNN_Graph<Cluster*> knn_graph(k, Util_Cluster::avg_distance);
    Lazy_ANN_Graph2<Cluster*> ann_graph2(k, 5, 20, Util_Cluster::avg_distance);
    Lazy_ANN_Graph<Cluster*> ann_graph(k, 5, 20, Util_Cluster::avg_distance);
    ae_graph = &dist_graph;
    //ae_graph = &knn_graph;
    //ae_graph = &ann_graph2;
    ae_graph = &ann_graph;
    ae_graph->set_parallel(enable_parallel);
    Cluster_Graph cls_graph(d, ae_graph);
    Cluster_Vector cls_vector(d);

    Cluster_Container *cls_container;
    cls_container = &cls_graph;
    //cls_container = &cls_vector;

    /*
    for(Data *d : data) cls_container->add_data(d);
    cls_container->init_clusters_fine_grained();
    return 0;
    */

    // choose algorithm
    Greedy_Joining gr_joining;
    gr_joining.set_cache(enable_cache);
    gr_joining.set_container(cls_container);
    gr_joining.set_parallel(enable_parallel);

    Greedy_Additive_EC gaec([d](Data *d1, Data *d2) -> float {
        float dist = Util_Math::euclidean_distance(*d1, *d2);
        return dist*dist - d*d;
    });
    gaec.set_parallel(enable_parallel);

    Clustering *clustering;
    clustering = &gaec;
    clustering = &gr_joining;
    /*
    repeat_and_write_csv(data, clustering, 0.0f, 1000.0f, 200.0f);
    return 0;
    */
    /*
    for(Data *d : data) cls_container->add_data(d);
    cls_container->init_clusters_fine_grained();
    std::cout << knn_graph.size() << std::endl;

    for(Cluster *cl : *cls_container)
    {
        std::vector<Cluster*> neighbours;
        cls_container->get_neighbours(neighbours, cl);
        std::cout << cl->to_string() << "\nchildren: ";
        //std::cout << neighbours.size() << std::endl;
        for(Cluster *neigh : neighbours)
        {
            std::cout << neigh->to_string() << ", ";
        }
        std::cout << std::endl << std::endl;
    }

    return 0;
    */

    timer.start();
    std::unordered_map<Data*, std::string> clustering_result = clustering->execute(data, d);
    
    double exec_time = timer.stop();

    std::unordered_set<std::string> labels;
    /*
    for(auto &entry : clustering_result)
    {
        //std::cout << "cl_label: " << entry.second << ", " << entry.first->to_string() << std::endl;
        labels.insert(entry.second);
    }*/

    std::string additional_info;

    additional_info += "execution time: " + std::to_string(exec_time) + "s\n";
    additional_info += "memory consumption: " + std::to_string(Memory_Usage::get_in_gb()) + " GB\n";
    additional_info += "number of clusters: " + std::to_string(labels.size()) + "\n";
    additional_info += "objective value: " + std::to_string(clustering->get_objective_value()) + "\n";
    additional_info += "underlying data structure: " + std::string(typeid(*ae_graph).name()) + "\n";
    additional_info += "number of calculating feature vectors: " + std::to_string(Util_Math::call_count.load()) + "\n";

    show_results(clustering_result, additional_info, false, false);

    // free stuff
    for(Data *d : data) delete d;
    data.clear();

    return 0;
}