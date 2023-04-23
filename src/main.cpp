#include <iostream>
#include <vector>
#include "parser/csv_parser.h"
#include "parser/ubyte_parser.h"
#include "data/data.h"
#include "algorithm/greedy_joining.h"
#include "eval/rand_index.h"
#include "eval/adjusted_rand_index.h"
#include "eval/silhouette_score.h"

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

    Greedy_Joining gr_joining;
    Clustering *clustering = &gr_joining;

    float d = 1.2f;
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