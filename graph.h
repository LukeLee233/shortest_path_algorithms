//
// Created by luke on 2020/9/12.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include "graph.h"
#include <memory>
#include <ostream>

using namespace std;

class GRAPH;

class BASE_PARSER
{
private:
    string name_;
public:
    explicit BASE_PARSER(string name): name_(std::move(name)){}

    virtual void operator()(string filename, GRAPH& g) = 0;

    string get_name() { return name_; }
};


class BHW_PARSER : public BASE_PARSER{
public:
    explicit BHW_PARSER(string name): BASE_PARSER(std::move(name)){}

    void operator()(string filename, GRAPH& g) override;
};



class GRAPH
{
    friend class BASE_PARSER;
public:
    vector<vector<int>> distance_matrix;
    vector<vector<int>> distance_matrix_bak;
    vector<vector<vector<int>>> path;
    int vertex_num{};
    int edge_num{};
    int arc_num{};
    string name;

private:
    unique_ptr<BASE_PARSER> parser;

public:
    GRAPH() = delete;
    ~GRAPH() = default;

    GRAPH(const string& filename, const string& type);

    inline bool valid() const{ return !distance_matrix.empty(); }
    friend ostream &operator<<(ostream &os, const GRAPH &graph);
};

void floyd(GRAPH& graph);

#endif //GRAPH_H
