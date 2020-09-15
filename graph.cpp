//
// Created by luke on 2020/9/12.
//

#include "graph.h"
#include "fstream"
#include "iostream"
#include <queue>
#include <functional>

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

/*!
 * Multi source multi target shortest path algorithm
 * one stipulation: non negative cost on every edge
 * dynamic programming idea
 * @param graph
 */
void floyd(GRAPH &graph)
{
    // save the relaxation node on path (i->j)
    vector<vector<int>> relax_node(graph.vertex_num + 1,vector<int>(graph.vertex_num+1,0));

    // O(n^3) to get minimum path
    for(int k = 1; k <= graph.vertex_num; k++){
        for(int u = 1;u <= graph.vertex_num;u++){
            for(int v = 1;v<=graph.vertex_num;v++){
                if(graph.distance_matrix[u][k] + graph.distance_matrix[k][v] < graph.distance_matrix[u][v]){
                    graph.distance_matrix[u][v] = graph.distance_matrix[u][k] + graph.distance_matrix[k][v];
                    relax_node[u][v] = k;
                }
            }
        }
    }

    function<vector<int>(int,int)> get_path = [&](int source,int terminal){
        if(relax_node[source][terminal] == 0){
            if(source == terminal)
                return vector<int>{source};
            else
                return vector<int>{source,terminal};
        }

        vector<int> first_seg;
        vector<int> second_seg;
        int bypass = relax_node[source][terminal];
        first_seg = get_path(source,bypass);
        second_seg = get_path(bypass,terminal);
        auto s2_begin = second_seg.begin();
        while(first_seg.back() == *s2_begin)
            s2_begin = next(s2_begin);

        first_seg.insert(first_seg.end(),s2_begin,second_seg.end());
        return first_seg;
    };


    for(int i = 1;i<=graph.vertex_num;i++){
        for(int j = 1;j<=graph.vertex_num;j++){
            if(i != j)
                graph.path[i][j] = get_path(i,j);
            else{
                if(graph.distance_matrix[i][j] < 0){
                    cout<< "Node "<< i << "in negative cycle!"<<endl;
                    exit(-1);
                }
                else{
                    graph.path[i][j].clear();
                }
            }
        }
    }

}
