#include <iostream>
#include <fstream>
#include "json.hpp"
#include "graph.h"

using namespace std;
using json = nlohmann::json;

bool test(const GRAPH& graph, const string& checkfile){
    ifstream fin;
    vector<vector<int>> distance_matrix;
    vector<vector<vector<int>>> path;
    fin.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        fin.open(checkfile);
        json j;
        fin >> j;
        j.at("distance").get_to(distance_matrix);
        j.at("path").get_to(path);
        fin.close();
    }
    catch (ifstream::failure& e) {
        cerr << "Exception opening/reading/closing "<<checkfile<<endl;
        exit(-1);
    }

    for(int i = 1;i<=graph.vertex_num;i++){
        for(int j = 1; j<= graph.vertex_num;j++){
            if(graph.distance_matrix[i][j] != distance_matrix[i-1][j-1] || graph.path[i][j] != path[i-1][j-1]){
                cout<<endl;
                cout << i<< "->" << j << endl;

                cout<<"graph info: "<< graph.distance_matrix[i][j] << endl;
                for(auto route : graph.path[i][j]) cout<<route<<"->";
                cout<<"\b\b"<<endl;
                cout<<endl;

                cout<<"result: "<< distance_matrix[i-1][j-1]<<endl;
                for(auto route : path[i-1][j-1]) cout<<route<<"->";
                cout<<"\b\b"<<endl;

                return false;
            }

        }
    }

    return true;
}

int main()
{
    string config_file("../config.json");
    string instance_file;
    string type;
    ifstream fin;

    fin.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        fin.open(config_file);
        json j;
        fin >> j;
        j.at("instance_file").get_to(instance_file);
        j.at("type").get_to(type);
        fin.close();
    }
    catch (ifstream::failure& e) {
        cerr << "Exception opening/reading/closing config file"<<endl;
        exit(-1);
    }

    GRAPH graph(instance_file,type);
    if(!graph.valid()){
        exit(-1);
    }

    cout<<graph;
    if(test(graph,"../Instances/test/cbmix23.json")){
        cout<<"result is correct\n";
    }else{
        cout<<"result is wrong\n";
    }

    return 0;
}

