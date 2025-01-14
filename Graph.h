#pragma once
#include <iostream>
#include <climits>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include "crow.h"
#include <json/json.h>
#include <stack>
#include <queue>
#include <exception>
#include <sstream>
using namespace std;



class Graph
{
private:
    // liste d'adjacence
    unordered_map<string, vector<pair<string, int>>> list;
   
    bool isDerected;// si le graphe est orienté ou non

public:
    // constructeur
    Graph(bool isDirected = false);
    // ajouter une arête
    void addEdge(const string& start, const string& end, int weight = 0);
    // pour recuperer la liste d'adjacence
    unordered_map<string, vector<pair<string, int>>> getList() const;

    //extraire les donnees du graph
    void extractGraphData(crow::json::rvalue& data);


    // pour afficher le graphe
    void printGraph() const;
};

