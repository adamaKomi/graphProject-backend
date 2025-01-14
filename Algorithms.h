#pragma once
#include "server_utils.h"



class Algorithms
{
public:

	// algorithme de dijskstra
	void dijkstra(Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end = "") const;
	// parcours en largeur
	void bfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end = "") const;
	// parcours en profondeur
	void dfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end = "") const;

	void a_star(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& goal) const;

	// fonction pour trouver le noeud de plus courte distance dans l'algorithme de dijkstra
	string getMinDistanceNode(const unordered_map<string, vector<pair<string, int>>>& list,
		const unordered_map<string, int>& distances,
		const unordered_map<string, bool>& visited
	) const;

	float heuristic(const string& node, const string& goal) const;
};

