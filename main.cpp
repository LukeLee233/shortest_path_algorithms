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
            int check_sum = 0;
            vector<int> path_rotate = graph.path[i][j];

            if(!path_rotate.empty()){
                // left rotate vector 1 time
                rotate(path_rotate.begin(),path_rotate.begin() + 1,path_rotate.end());
                for(int pos = 0; pos<(int)path_rotate.size() - 1; pos++)
                    check_sum += graph.distance_matrix_bak[graph.path[i][j][pos]][path_rotate[pos]];
            }

            if(graph.distance_matrix[i][j] != distance_matrix[i-1][j-1] || check_sum != distance_matrix[i-1][j-1]){
                cout<<endl;
                cout << i<< "->" << j << endl;

                cout<<"graph info: "<< graph.distance_matrix[i][j] << endl;
                for(auto route : graph.path[i][j]) cout<<route<<"->";
                cout<<"\b\b"<<endl;
                cout<<endl;

                cout<<"check sum: "<< check_sum <<endl;
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
    vector<string> instance_file;
    vector<string> result_file;
    string type;
    ifstream fin;

    fin.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        fin.open(config_file);
        json j;
        fin >> j;
        j.at("type").get_to(type);
        j.at("instance_file").get_to(instance_file);
        j.at("result_file").get_to(result_file);
        fin.close();
    }
    catch (ifstream::failure& e) {
        cerr << "Exception opening/reading/closing config file"<<endl;
        exit(-1);
    }

    for(int i = 0;i<instance_file.size();i++){
        GRAPH graph(instance_file[i],type);
        if(!graph.valid()){
            exit(-1);
        }

        cout<<graph<<endl;

        if(test(graph,result_file[i])){
            cout<<"result is correct\n";
        }else{
            cout<<"result is wrong\n";
            exit(-1);
        }
    }


    return 0;
}

