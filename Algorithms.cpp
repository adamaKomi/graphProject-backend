#pragma once
#include "Algorithms.h"



void Algorithms::dijkstra(Graph& graph, const std::string& start, server& ws_server, websocketpp::connection_hdl hdl) const {
	// R�cup�rer la liste d'adjacence du graphe
	auto list = graph.getList();

	// V�rifications initiales
	if (list.empty() || start.empty()) {
		std::cout << "Graphe ou le n�ud de d�part vide" << std::endl;
		return;
	}
	if (list.find(start) == list.end()) {
		std::cout << "Le n�ud '" << start << "' n'existe pas !!!" << std::endl;
		return;
	}

	std::cout << "Le n�ud de depart " << start << std::endl;

	std::cout << "Le graphe : " << std::endl;
	graph.printGraph();
	std::cout << "Hello 1 " << std::endl;
	std::unordered_map<std::string, int> distances;
	std::unordered_map<std::string, bool> visited;
	std::unordered_map<std::string, std::string> previous;

	// Initialiser les distances et les n�uds visit�s
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

		visited[minNode] = true;  // Marquer le n�ud comme visit�
		visitedNodesJson.push_back(minNode);  // Ajouter le n�ud visit� au JSON

		// Mettre � jour les distances pour les voisins du n�ud
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

		// Envoyer les n�uds visit�s au client via WebSocket
		try {
			std::cout << "Mes visited nodes : "<< visitedNodesJson.dump() << std::endl;
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
			std::cout << "Hello 4 " << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
			std::cout << "Hello 5 " << std::endl;
		}
	}
}





void Algorithms::bfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl) const
{
	// R�cup�rer la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// V�rifier si le graphe ou le n�ud de d�part sont vides
	if (list.empty() || start.empty())
	{
		std::cout << "Graphe ou le n�ud de d�part vide" << std::endl;
		return;
	}

	// V�rifier si le n�ud de d�part existe dans le graphe
	if (list.find(start) == list.end())
	{
		std::cout << "Le n�ud '" << start << "' n'existe pas !!!" << std::endl;
		return;
	}

	// Affichage de l'ent�te
	std::cout << std::endl;
	std::cout << "================================================================= " << std::endl;
	std::cout << "---------------------PARCOURS EN LARGEUR-------------------------" << std::endl;
	std::cout << "Sommet de d�part : " << start << std::endl;
	std::cout << "================================================================= " << std::endl;

	// Initialisation des structures de donn�es
	unordered_map<string, int> distances;   // Distance de chaque n�ud par rapport au n�ud de d�part
	unordered_map<string, string> colors;  // Couleurs des n�uds : blanc (non visit�), gris (en cours), noir (trait�)
	unordered_map<string, string> parent;  // Pr�d�cesseur de chaque n�ud pour reconstruire les chemins

	queue<string> Queue;  // File pour g�rer les n�uds � visiter

	// Initialisation des distances, couleurs et parents pour tous les n�uds
	for (auto elem : list)
	{
		distances[elem.first] = INT_MAX; // Distance infinie par d�faut
		colors[elem.first] = "blanc";   // Tous les n�uds sont initialement blancs
		parent[elem.first] = "";        // Pas de pr�d�cesseur initial
	}

	// Initialiser le n�ud de d�part
	distances[start] = 0;   // La distance du n�ud de d�part � lui-m�me est 0
	colors[start] = "gris"; // Le n�ud de d�part devient gris (en cours de traitement)
	parent[start] = "";     // Pas de pr�d�cesseur pour le n�ud de d�part
	Queue.push(start);      // Ajouter le n�ud de d�part � la file

	int it = 0; // Compteur pour num�roter les it�rations

	json visitedNodesJson = json::array();  // Initialisation du tableau JSON

	// Boucle principale : traiter les n�uds tant que la file n'est pas vide
	while (!Queue.empty())
	{
		// R�cup�rer et retirer le premier n�ud de la file
		string currentNode = Queue.front();
		Queue.pop();

		// Parcourir tous les successeurs du n�ud actuel
		for (auto elem : list[currentNode])
		{
			// Si un successeur n'est pas encore visit� (blanc)
			if (colors[elem.first] == "blanc")
			{
				// Mettre � jour sa couleur, distance et pr�d�cesseur
				colors[elem.first] = "gris";
				distances[elem.first] = distances[currentNode] + 1;
				parent[elem.first] = currentNode;

				// Ajouter le successeur � la file pour le traiter plus tard
				Queue.push(elem.first);
			}
		}

		// Marquer le n�ud actuel comme trait� (noir)
		colors[currentNode] = "noir";

		visitedNodesJson.push_back(currentNode);


		// Envoyer les n�uds visit�s au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
			std::cout << "Hello 5 " << std::endl;
		}

		//// Affichage de l'�tat des n�uds apr�s chaque it�ration
		//std::cout << "\n------------------ Iteration " << it++ << " ------------------\n";

		//// Afficher les n�uds noirs (trait�s)
		//std::cout << "N�uds noirs :\n" << std::endl;
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

		//// Afficher les n�uds gris (en cours de traitement)
		//std::cout << "N�uds gris :\n" << std::endl;
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

		//// Afficher les n�uds blancs (non visit�s)
		//std::cout << "N�uds blancs :\n" << std::endl;
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
	// R�cup�ration de la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// V�rification si le graphe ou le n�ud de d�part est vide
	if (list.empty() || start.empty()) {
		std::cout << "Graphe ou le noeud de depart vide" << std::endl;
		return;
	}

	// V�rification si le n�ud de d�part existe dans le graphe
	if (list.find(start) == list.end()) {
		std::cout << "Le noeud '" << start << "' n'existe pas!!!" << std::endl;
		return;
	}

	// Affichage de l'ent�te pour le parcours en profondeur
	std::cout << std::endl;
	std::cout << "================================================================= " << std::endl;
	std::cout << "---------------------PARCOURS EN PROFONDEUR---------------------" << std::endl;
	std::cout << "Sommet de depart : " << start << std::endl;
	std::cout << "================================================================= " << std::endl;

	// Initialisation des structures de donn�es
	unordered_map<string, string> colors; // Suivi des couleurs des n�uds (blanc, gris, noir)
	unordered_map<string, string> parent; // Suivi du parent de chaque n�ud
	stack<string> Stack; // Pile pour la gestion des n�uds � visiter

	// Initialisation des n�uds du graphe
	for (auto elem : list) {
		colors[elem.first] = "blanc"; // Tous les n�uds sont initialement blancs
		parent[elem.first] = "";      // Pas de parent initialement
	}

	// Initialisation du n�ud de d�part
	colors[start] = "gris"; // Le n�ud de d�part devient gris
	parent[start] = "";     // Pas de parent pour le n�ud de d�part
	Stack.push(start);      // Ajout du n�ud de d�part dans la pile

	//int it = 0; // Pour num�roter les it�rations

	json visitedNodesJson = json::array();  // Initialisation du tableau JSON

	// Parcours en profondeur
	while (!Stack.empty()) {
		string currentNode = Stack.top(); // R�cup�ration du n�ud au sommet de la pile
		Stack.pop(); // Suppression du n�ud de la pile

		bool foundNeighbor = false; // Indique si un voisin blanc a �t� trouv�

		// Exploration des voisins du n�ud courant
		for (auto elem : list[currentNode]) {
			if (colors[elem.first] == "blanc") {
				colors[elem.first] = "gris";         // Marque le voisin comme gris
				parent[elem.first] = currentNode;   // D�finit le n�ud courant comme parent
				Stack.push(elem.first);             // Ajoute le voisin � la pile
				foundNeighbor = true;               // Indique qu'un voisin a �t� trouv�
			}
		}

		colors[currentNode] = "noir"; // Marque le n�ud courant comme visit� (noir)

		visitedNodesJson.push_back(currentNode);


		// Envoyer les n�uds visit�s au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
			std::cout << "Hello 5 " << std::endl;
		}



		//// Affichage de l'�tat des n�uds pour cette it�ration
		//std::cout << "\n------------------ Iteration " << it++ << " ------------------\n";

		//// Afficher les n�uds noirs
		//std::cout << "Noeuds noirs :\n";
		//for (auto x : list) {
		//	if (colors[x.first] == "noir") {
		//		std::cout << "- " << x.first << " [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}

		//// Afficher les n�uds gris
		//std::cout << "\nNoeuds gris :\n";
		//for (auto x : list) {
		//	if (colors[x.first] == "gris") {
		//		std::cout << "- " << x.first << " [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}

		//// Afficher les n�uds blancs
		//std::cout << "\nNoeuds blancs :\n";
		//for (auto x : list) {
		//	if (colors[x.first] == "blanc") {
		//		std::cout << "- " << x.first << " [Couleur: " << colors[x.first] << "]";
		//		if (!parent[x.first].empty()) std::cout << " [Parent: " << parent[x.first] << "]";
		//		std::cout << std::endl;
		//	}
		//}

		//// Si aucun voisin blanc trouv�, afficher le chemin complet jusqu'au n�ud courant
		//if (!foundNeighbor) {
		//	std::cout << "\nChemin complet jusqu'au noeud '" << currentNode << "': ";
		//	string pathNode = currentNode;
		//	vector<string> path;

		//	// Reconstruction du chemin depuis le n�ud courant jusqu'au n�ud de d�part
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
	const unordered_map<string, bool>& visited // pour verifier si un noeud est visit�
) const
{
	// Initialisation du n�ud avec la distance minimale
	string minNode = "";
	int minDist = INT_MAX; // Distance minimale initialis�e � la valeur maximale d'un entier

	// Parcours de tous les n�uds de la liste d'adjacence
	for (auto x : list) {
		// V�rifie si le n�ud n'a pas �t� visit� et si sa distance est inf�rieure � la distance minimale trouv�e jusqu'ici
		if (!visited.at(x.first) && distances.at(x.first) < minDist) {
			minDist = distances.at(x.first); // Met � jour la distance minimale
			minNode = x.first;               // Met � jour le n�ud ayant cette distance minimale
		}
	}

	// Retourne le n�ud avec la distance minimale
	return minNode;
}


