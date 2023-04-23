#include "data/graph/graph.h"

template<typename T> Node<T>* Graph<T>::get_node(T *t)
{
    return node_map[t];
}

template<typename T> void Graph<T>::add_node(T *t)
{
    Node<T> *node = new Node<T>(t);
    node_map[t] = node;
}

template<typename T> void Graph<T>::remove_node(T *t)
{
    Node<T> *node = get_node(t);
    std::unordered_set<Node<T>*> children_set = node->get_children();
    std::vector<Node<T>*> children(children_set.begin(), children_set.end());

    for(Node<T> *next : children)
    {
        remove_edge(t, next->get_value());
    }
}

template<typename T> void Graph<T>::add_edge(T *t1, T *t2)
{
    Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
    n1->get_children.insert(n2);
    n2->get_children.insert(n1);
}

template<typename T> void Graph<T>::remove_edge(T *t1, T *t2)
{
    Node<T> *n1 = get_node(t1), *n2 = get_node(t2);
    n1->get_children.erase(n2);
    n2->get_children.erase(n1);
}

template<typename T> void Graph<T>::get_all_values(std::vector<T*> &vec)
{
    vec.reserve(node_map.size());

    for(auto &entry : node_map)
    {
        vec.push_back(entry.second);
    }
}