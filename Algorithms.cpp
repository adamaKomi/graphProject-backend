#pragma once
#include "Algorithms.h"



void Algorithms::dijkstra(Graph& graph, const std::string& start, server& ws_server, websocketpp::connection_hdl hdl) const {
	// Récupérer la liste d'adjacence du graphe
	auto list = graph.getList();

	// Vérifications initiales
	if (list.empty() || start.empty()) {
		std::cout << "Graphe ou le nœud de départ vide" << std::endl;
		return;
	}
	if (list.find(start) == list.end()) {
		std::cout << "Le nœud '" << start << "' n'existe pas !!!" << std::endl;
		return;
	}

	std::cout << "Le nœud de depart " << start << std::endl;

	std::cout << "Le graphe : " << std::endl;
	graph.printGraph();
	std::cout << "Hello 1 " << std::endl;
	std::unordered_map<std::string, int> distances;
	std::unordered_map<std::string, bool> visited;
	std::unordered_map<std::string, std::string> previous;

	// Initialiser les distances et les nœuds visités
	for (const auto& elem : list) {
		distances[elem.first] = INT_MAX;
		visited[elem.first] = false;
	}
	distances[start] = 0;
	previous[start] = "";
	std::cout << "Hello 2 " << std::endl;
	json visitedNodesJson = json::array();  // Initialisation du tableau JSON
	// Effectuer l'algorithme de Dijkstra
	std::cout << "Hello 3 " << std::endl;
	while (true) {
		std::cout << "Hello A ";
		std::string minNode = getMinDistanceNode(list, distances, visited);
		if (minNode.empty()) break;

		visited[minNode] = true;  // Marquer le nœud comme visité
		visitedNodesJson.push_back(minNode);  // Ajouter le nœud visité au JSON

		// Mettre à jour les distances pour les voisins du nœud
		for (const auto& successor : list[minNode]) {
			std::cout << "Hello B ";
			if (!visited[successor.first]) {
				int minDist = distances[minNode] + successor.second;
				if (minDist < distances[successor.first]) {
					distances[successor.first] = minDist;
					previous[successor.first] = minNode;
				}
			}
		}

		// Envoyer les nœuds visités au client via WebSocket
		try {
			std::cout << "Mes visited nodes : "<< visitedNodesJson.dump() << std::endl;
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
			std::cout << "Hello 4 " << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
			std::cout << "Hello 5 " << std::endl;
		}
	}
}





void Algorithms::bfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl) const
{
	// Récupérer la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// Vérifier si le graphe ou le nœud de départ sont vides
	if (list.empty() || start.empty())
	{
		std::cout << "Graphe ou le nœud de départ vide" << std::endl;
		return;
	}

	// Vérifier si le nœud de départ existe dans le graphe
	if (list.find(start) == list.end())
	{
		std::cout << "Le nœud '" << start << "' n'existe pas !!!" << std::endl;
		return;
	}

	// Affichage de l'entête
	std::cout << std::endl;
	std::cout << "================================================================= " << std::endl;
	std::cout << "---------------------PARCOURS EN LARGEUR-------------------------" << std::endl;
	std::cout << "Sommet de départ : " << start << std::endl;
	std::cout << "================================================================= " << std::endl;

	// Initialisation des structures de données
	unordered_map<string, int> distances;   // Distance de chaque nœud par rapport au nœud de départ
	unordered_map<string, string> colors;  // Couleurs des nœuds : blanc (non visité), gris (en cours), noir (traité)
	unordered_map<string, string> parent;  // Prédécesseur de chaque nœud pour reconstruire les chemins

	queue<string> Queue;  // File pour gérer les nœuds à visiter

	// Initialisation des distances, couleurs et parents pour tous les nœuds
	for (auto elem : list)
	{
		distances[elem.first] = INT_MAX; // Distance infinie par défaut
		colors[elem.first] = "blanc";   // Tous les nœuds sont initialement blancs
		parent[elem.first] = "";        // Pas de prédécesseur initial
	}

	// Initialiser le nœud de départ
	distances[start] = 0;   // La distance du nœud de départ à lui-même est 0
	colors[start] = "gris"; // Le nœud de départ devient gris (en cours de traitement)
	parent[start] = "";     // Pas de prédécesseur pour le nœud de départ
	Queue.push(start);      // Ajouter le nœud de départ à la file

	int it = 0; // Compteur pour numéroter les itérations

	json visitedNodesJson = json::array();  // Initialisation du tableau JSON

	// Boucle principale : traiter les nœuds tant que la file n'est pas vide
	while (!Queue.empty())
	{
		// Récupérer et retirer le premier nœud de la file
		string currentNode = Queue.front();
		Queue.pop();

		// Parcourir tous les successeurs du nœud actuel
		for (auto elem : list[currentNode])
		{
			// Si un successeur n'est pas encore visité (blanc)
			if (colors[elem.first] == "blanc")
			{
				// Mettre à jour sa couleur, distance et prédécesseur
				colors[elem.first] = "gris";
				distances[elem.first] = distances[currentNode] + 1;
				parent[elem.first] = currentNode;

				// Ajouter le successeur à la file pour le traiter plus tard
				Queue.push(elem.first);
			}
		}

		// Marquer le nœud actuel comme traité (noir)
		colors[currentNode] = "noir";

		visitedNodesJson.push_back(currentNode);


		// Envoyer les nœuds visités au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
			std::cout << "Hello 5 " << std::endl;
		}

		//// Affichage de l'état des nœuds après chaque itération
		//std::cout << "\n------------------ Iteration " << it++ << " ------------------\n";

		//// Afficher les nœuds noirs (traités)
		//std::cout << "Nœuds noirs :\n" << std::endl;
		//for (auto x : list)
		//{
		//	if (colors[x.first] == "noir")
		//	{
		//		std::cout << "- " << x.first << " [Distance: " << distances[x.first] << "] [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}
		//std::cout << std::endl;

		//// Afficher les nœuds gris (en cours de traitement)
		//std::cout << "Nœuds gris :\n" << std::endl;
		//for (auto x : list)
		//{
		//	if (colors[x.first] == "gris")
		//	{
		//		std::cout << "- " << x.first << " [Distance: " << distances[x.first] << "] [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}
		//std::cout << std::endl;

		//// Afficher les nœuds blancs (non visités)
		//std::cout << "Nœuds blancs :\n" << std::endl;
		//for (auto x : list)
		//{
		//	if (colors[x.first] == "blanc")
		//	{
		//		std::cout << "- " << x.first << " [Distance: " << distances[x.first] << "] [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}
	}
}



void Algorithms::dfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl) const {
	// Récupération de la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// Vérification si le graphe ou le nœud de départ est vide
	if (list.empty() || start.empty()) {
		std::cout << "Graphe ou le noeud de depart vide" << std::endl;
		return;
	}

	// Vérification si le nœud de départ existe dans le graphe
	if (list.find(start) == list.end()) {
		std::cout << "Le noeud '" << start << "' n'existe pas!!!" << std::endl;
		return;
	}

	// Affichage de l'entête pour le parcours en profondeur
	std::cout << std::endl;
	std::cout << "================================================================= " << std::endl;
	std::cout << "---------------------PARCOURS EN PROFONDEUR---------------------" << std::endl;
	std::cout << "Sommet de depart : " << start << std::endl;
	std::cout << "================================================================= " << std::endl;

	// Initialisation des structures de données
	unordered_map<string, string> colors; // Suivi des couleurs des nœuds (blanc, gris, noir)
	unordered_map<string, string> parent; // Suivi du parent de chaque nœud
	stack<string> Stack; // Pile pour la gestion des nœuds à visiter

	// Initialisation des nœuds du graphe
	for (auto elem : list) {
		colors[elem.first] = "blanc"; // Tous les nœuds sont initialement blancs
		parent[elem.first] = "";      // Pas de parent initialement
	}

	// Initialisation du nœud de départ
	colors[start] = "gris"; // Le nœud de départ devient gris
	parent[start] = "";     // Pas de parent pour le nœud de départ
	Stack.push(start);      // Ajout du nœud de départ dans la pile

	//int it = 0; // Pour numéroter les itérations

	json visitedNodesJson = json::array();  // Initialisation du tableau JSON

	// Parcours en profondeur
	while (!Stack.empty()) {
		string currentNode = Stack.top(); // Récupération du nœud au sommet de la pile
		Stack.pop(); // Suppression du nœud de la pile

		bool foundNeighbor = false; // Indique si un voisin blanc a été trouvé

		// Exploration des voisins du nœud courant
		for (auto elem : list[currentNode]) {
			if (colors[elem.first] == "blanc") {
				colors[elem.first] = "gris";         // Marque le voisin comme gris
				parent[elem.first] = currentNode;   // Définit le nœud courant comme parent
				Stack.push(elem.first);             // Ajoute le voisin à la pile
				foundNeighbor = true;               // Indique qu'un voisin a été trouvé
			}
		}

		colors[currentNode] = "noir"; // Marque le nœud courant comme visité (noir)

		visitedNodesJson.push_back(currentNode);


		// Envoyer les nœuds visités au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
			std::cout << "Hello 5 " << std::endl;
		}



		//// Affichage de l'état des nœuds pour cette itération
		//std::cout << "\n------------------ Iteration " << it++ << " ------------------\n";

		//// Afficher les nœuds noirs
		//std::cout << "Noeuds noirs :\n";
		//for (auto x : list) {
		//	if (colors[x.first] == "noir") {
		//		std::cout << "- " << x.first << " [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}

		//// Afficher les nœuds gris
		//std::cout << "\nNoeuds gris :\n";
		//for (auto x : list) {
		//	if (colors[x.first] == "gris") {
		//		std::cout << "- " << x.first << " [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}

		//// Afficher les nœuds blancs
		//std::cout << "\nNoeuds blancs :\n";
		//for (auto x : list) {
		//	if (colors[x.first] == "blanc") {
		//		std::cout << "- " << x.first << " [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}

		//// Si aucun voisin blanc trouvé, afficher le chemin complet jusqu'au nœud courant
		//if (!foundNeighbor) {
		//	std::cout << "\nChemin complet jusqu'au noeud '" << currentNode << "': ";
		//	string pathNode = currentNode;
		//	vector<string> path;

		//	// Reconstruction du chemin depuis le nœud courant jusqu'au nœud de départ
		//	while (!pathNode.empty()) {
		//		path.push_back(pathNode);
		//		pathNode = parent[pathNode];
		//	}

		//	// Affichage du chemin dans l'ordre
		//	reverse(path.begin(), path.end());
		//	for (size_t i = 0; i < path.size(); ++i) {
		//		std::cout << path[i];
		//		if (i < path.size() - 1) std::cout << " -> ";
		//	}
		//	std::cout << std::endl;
		//}
	}
}


string Algorithms::getMinDistanceNode(
	const unordered_map<string, vector<pair<string, int>>>& list,// la list d'adjacence
	const unordered_map<string, int>& distances, // les distances
	const unordered_map<string, bool>& visited // pour verifier si un noeud est visité
) const
{
	// Initialisation du nœud avec la distance minimale
	string minNode = "";
	int minDist = INT_MAX; // Distance minimale initialisée à la valeur maximale d'un entier

	// Parcours de tous les nœuds de la liste d'adjacence
	for (auto x : list) {
		// Vérifie si le nœud n'a pas été visité et si sa distance est inférieure à la distance minimale trouvée jusqu'ici
		if (!visited.at(x.first) && distances.at(x.first) < minDist) {
			minDist = distances.at(x.first); // Met à jour la distance minimale
			minNode = x.first;               // Met à jour le nœud ayant cette distance minimale
		}
	}

	// Retourne le nœud avec la distance minimale
	return minNode;
}


