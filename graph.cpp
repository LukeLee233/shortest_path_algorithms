//
// Created by luke on 2020/9/12.
//

#include "graph.h"
#include "fstream"
#include "iostream"
#include <queue>
#include <algorithm>

const int inf = 0x3f3f3f3f;

void BHW_PARSER::operator()(string filename, GRAPH &g)
{
    try {
        ifstream fin;
        fin.open(filename);
        string contenu, useless;

        fin >> useless;
        fin >> g.name;
        for (int i = 0; i < 5; i++)
            getline(fin, contenu);

        fin >> useless >> g.vertex_num;
        fin >> useless >> g.edge_num;
        fin >> useless >> g.arc_num;
        getline(fin, contenu);
        int require_edge, require_arc;

        getline(fin, contenu);
        fin >> useless >> useless >> require_edge;
        fin >> useless >> useless >> require_arc;

        g.distance_matrix = vector<vector<int>>(
            g.vertex_num + 1, vector<int>(g.vertex_num + 1, inf));

        g.path = vector<vector<vector<int>>>(g.vertex_num + 1,
                                             vector<vector<int>>(g.vertex_num + 1,
                                                                 vector<int>()));
        for (int i = 1; i <= g.vertex_num; i++) {
            g.distance_matrix[i][i] = 0;
            for (int j = 1; j <= g.vertex_num; j++) {
                g.path[i][j].push_back(i);
            }
        }

        while (getline(fin, contenu) && contenu.find("ReE") == string::npos) {}
        for (int i = 0; i < require_edge; i++) {
            int start, end;
            fin >> useless >> start >> end >> g.distance_matrix[start][end];
            g.distance_matrix[end][start] = g.distance_matrix[start][end];
            getline(fin, contenu);
        }

        while (getline(fin, contenu) && contenu.find("EDGE") == string::npos) {}
        for (int i = 0; i < g.edge_num - require_edge; i++) {
            int start, end;
            fin >> useless >> start >> end >> g.distance_matrix[start][end];
            g.distance_matrix[end][start] = g.distance_matrix[start][end];
            getline(fin, contenu);
        }

        while (getline(fin, contenu) && contenu.find("ReA") == string::npos) {}
        for (int i = 0; i < require_arc; i++) {
            int start, end;
            fin >> useless >> start >> end >> g.distance_matrix[start][end];
            getline(fin, contenu);
        }

        while (getline(fin, contenu) && contenu.find("ARC") == string::npos) {}
        for (int i = 0; i < g.arc_num - require_arc; i++) {
            int start, end;
            fin >> useless >> start >> end >> g.distance_matrix[start][end];
            getline(fin, contenu);
        }

        g.distance_matrix_bak = g.distance_matrix;
        fin.close();
    }
    catch (ifstream::failure &e) {
        cerr << "Exception opening/reading/closing instance file" << endl;
        g.distance_matrix.clear();
    }

}

GRAPH::GRAPH(const string &filename, const string &type)
{
    if (type == "bhw") {
        unique_ptr<BASE_PARSER> tmp(new BHW_PARSER(filename));
        this->parser = move(tmp);
    }
    else {
        this->distance_matrix.clear();
    }

    ifstream fin;

    fin.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        (this->parser)->operator()(filename, *this);
    }
    catch (ifstream::failure &e) {
        cerr << "Exception opening/reading/closing instance file" << endl;
        this->distance_matrix.clear();
    }

}

ostream &operator<<(ostream &os, const GRAPH &graph)
{
    if (graph.distance_matrix.empty()) {
        os << "Graph" << graph.name << "invalid!" << endl;
    }
    else {
        os << "Graph " << graph.name << " valid" << endl;
        os << "distance_matrix:\n";
        os << "\t";
        for (int i = 1; i <= graph.vertex_num; i++)
            os << i << "\t";
        os << "\n";

        for (int i = 1; i <= graph.vertex_num; i++) {
            os << i << '\t';
            for (int j = 1; j <= graph.vertex_num; j++) {
                if (graph.distance_matrix[i][j] == inf)
                    os << "INF" << '\t';
                else
                    os << graph.distance_matrix[i][j] << '\t';
            }
            os << '\n';
        }

    }

    return os;
}

void bellman(GRAPH &graph)
{
    struct Edge
    {
        int source, sink;
        int cost;
        Edge(int source, int sink, int cost)
            : source(source), sink(sink), cost(cost)
        {}
    };

    vector<Edge> edges;
    for (int i = 1; i <= graph.vertex_num; i++) {
        for (int j = 1; j <= graph.vertex_num; j++) {
            if (graph.distance_matrix_bak[i][j] != inf && graph.distance_matrix_bak[i][j] != 0)
                edges.emplace_back(Edge(i, j, graph.distance_matrix_bak[i][j]));
        }
    }

    for (int i = 1; i <= graph.vertex_num; i++) {
        vector<int> distance(graph.vertex_num + 1, inf);

        // this record the penultimate vertex in the shortest path leading to target node
        vector<int> predecessor(graph.vertex_num + 1, 0);
        distance[i] = 0;

        for (int k = 0; k < graph.vertex_num; k++) {
            bool relax_success = false;
            for (auto const &edge : edges) {
                // if negative weight exists, this check is necessary to avoid
                // ∞-1, ∞-2 etc.
                if (distance[edge.source] < inf) {
                    if (distance[edge.sink] > distance[edge.source] + edge.cost) {
                        distance[edge.sink] = distance[edge.source] + edge.cost;
                        predecessor[edge.sink] = edge.source;
                        relax_success = true;
                    }
                }
            }

            if (!relax_success)
                break;

            if (k == graph.vertex_num - 1) {
                cout << "Graph " << graph.name << " has negative cycle!" << endl;
                graph.distance_matrix.clear();
                return;
            }
        }

        graph.distance_matrix[i] = distance;
        for (int j = 1; j <= graph.vertex_num; j++) {
            if (i == j)
                graph.path[i][j] = vector<int>();
            else {
                graph.path[i][j].clear();
                for (int cur = j; cur != 0; cur = predecessor[cur]) {
                    graph.path[i][j].push_back(cur);
                }
                reverse(graph.path[i][j].begin(), graph.path[i][j].end());
            }
        }
    }

}
