//
// Created by luke on 2020/9/12.
//

#include "graph.h"
#include "fstream"
#include "iostream"
#include <queue>

const int inf = 0x3f3f3f3f;

void BHW_PARSER::operator()(string filename, GRAPH &g)
{
    try {
        ifstream fin;
        fin.open(filename);
        string contenu,useless;

        fin>>useless;
        fin>>g.name;
        for(int i = 0; i < 5;i++)
            getline(fin,contenu);

        fin>>useless>>g.vertex_num;
        fin>>useless>>g.edge_num;
        fin>>useless>>g.arc_num;
        getline(fin,contenu);
        int require_edge, require_arc;

        getline(fin,contenu);
        fin>>useless>>useless>>require_edge;
        fin>>useless>>useless>>require_arc;

        g.distance_matrix = vector<vector<int>>(
            g.vertex_num + 1,vector<int>(g.vertex_num + 1,inf));

        g.path = vector<vector<vector<int>>>(g.vertex_num + 1,
                                             vector<vector<int>>(g.vertex_num+1,
                                                 vector<int>()));
        for(int i = 1 ; i<=g.vertex_num; i++){
            g.distance_matrix[i][i] = 0;
            for(int j = 1; j <= g.vertex_num; j++){
                g.path[i][j].push_back(i);
            }
        }

        while (getline(fin,contenu) && contenu.find("ReE") == string::npos){}
        for(int i = 0;i<require_edge;i++){
            int start,end;
            fin>>useless>>start>>end>>g.distance_matrix[start][end];
            g.distance_matrix[end][start] = g.distance_matrix[start][end];
            getline(fin,contenu);
        }

        while (getline(fin,contenu) && contenu.find("EDGE") == string::npos){}
        for(int i = 0; i<g.edge_num - require_edge; i++){
            int start,end;
            fin>>useless>>start>>end>>g.distance_matrix[start][end];
            g.distance_matrix[end][start] = g.distance_matrix[start][end];
            getline(fin,contenu);
        }

        while (getline(fin,contenu) && contenu.find("ReA") == string::npos){}
        for(int i = 0; i<require_arc ; i++){
            int start,end;
            fin>>useless>>start>>end>>g.distance_matrix[start][end];
            getline(fin,contenu);
        }

        while (getline(fin,contenu) && contenu.find("ARC") == string::npos){}
        for(int i = 0; i<g.arc_num - require_arc ; i++){
            int start,end;
            fin>>useless>>start>>end>>g.distance_matrix[start][end];
            getline(fin,contenu);
        }

        g.distance_matrix_bak = g.distance_matrix;
        fin.close();
    }
    catch (ifstream::failure& e) {
        cerr << "Exception opening/reading/closing instance file" << endl;
        g.distance_matrix.clear();
    }

}

GRAPH::GRAPH(const string& filename, const string& type)
{
    if(type == "bhw"){
        unique_ptr<BASE_PARSER> tmp(new BHW_PARSER(filename));
        this->parser = move(tmp);
    }else{
        this->distance_matrix.clear();
    }

    ifstream fin;

    fin.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        (this->parser)->operator()(filename,*this);
    }
    catch (ifstream::failure& e) {
        cerr<<"Exception opening/reading/closing instance file"<<endl;
        this->distance_matrix.clear();
    }

}

/*!
 * Single source multi target shortest path algorithm
 * one stipulation: non negative cost on every edge
 * greedy idea
 * @param graph
 */
void dijkstra(GRAPH &graph)
{
    typedef pair<int,int> MIN_NODE; // (node_id, cost)
    vector<vector<bool>> direct_connect(graph.vertex_num + 1,vector<bool>
        (graph.vertex_num + 1, false));

    for (int i = 1; i <= graph.vertex_num; i++){
        for(int j = 1;j<=graph.vertex_num;j++){
            if(graph.distance_matrix[i][j] != inf)
                direct_connect[i][j] = true;
        }
    }

    for (int i = 1; i <= graph.vertex_num; i++){
        vector<bool> visited(graph.vertex_num + 1, false);
        visited[i] = true;

        vector<int> distance = graph.distance_matrix[i];

        for(int iter = 1; iter < graph.vertex_num; iter++){
            MIN_NODE min_node{0, inf};

            // choosing phase
            for(int j = 1; j <= graph.vertex_num;j++){
                if(visited[j])
                    continue;

                if(min_node.second > distance[j])
                    min_node = { j,distance[j] };
            }

            if(min_node.second == inf) // means node i cannot reach any other nodes further
                break;

            int nearest_node = min_node.first;
            visited[nearest_node] = true;
            graph.path[i][nearest_node].push_back(nearest_node);

            // relaxation phase
            for(int j = 1; j<= graph.vertex_num;j++){
                if(visited[j])
                    continue;

                if(direct_connect[nearest_node][j] && min_node.second + graph.distance_matrix[nearest_node][j] < distance[j]){
                    distance[j] = min_node.second + graph.distance_matrix[nearest_node][j];
                    graph.path[i][j] = graph.path[i][nearest_node];
                }
            }

        }
        graph.distance_matrix[i].swap(distance);


    }

    for(int i = 1;i<=graph.vertex_num;i++){
        graph.path[i][i].clear();
        for(int j = 1;j < graph.vertex_num;j++){
            if(graph.distance_matrix[i][j] == inf)
                graph.path[i][j].clear();
        }
    }

}

ostream &operator<<(ostream &os, const GRAPH &graph)
{
    if(graph.distance_matrix.empty()){
        os << "Graph" << graph.name << "invalid!"<< endl;
    }
    else{
        os << "Graph " << graph.name << " valid"<< endl;
        os << "distance_matrix:\n";
        os << "\t";
        for(int i = 1;i<=graph.vertex_num;i++)
            os << i << "\t";
        os<<"\n";

        for(int i = 1;i<=graph.vertex_num;i++){
            os << i <<'\t';
            for(int j = 1;j<=graph.vertex_num;j++){
                if(graph.distance_matrix[i][j] == inf)
                    os << "INF" << '\t';
                else
                    os << graph.distance_matrix[i][j] << '\t';
            }
            os << '\n';
        }

    }

    return os;
}
