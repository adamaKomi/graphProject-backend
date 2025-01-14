#pragma once
#include "Algorithms.h"



void Algorithms::dijkstra(Graph& graph, const std::string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end) const {
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
	json visitedNodesJson = json::array();  // Initialisation du tableau JSON
	// Effectuer l'algorithme de Dijkstra

	while (true) {
		std::string minNode = getMinDistanceNode(list, distances, visited);
		if (minNode.empty()) break;

		visited[minNode] = true;  // Marquer le nœud comme visité
		visitedNodesJson.push_back(minNode);  // Ajouter le nœud visité au JSON

		// verifier si le chemin est trouvé
		if (end == minNode) {

			vector<std::string> path, subPath;
			std::string tmp = minNode;

			json confirmation = json::object();
			confirmation["pathFound"] = true;

			// Envoyer lca confirmation de la fin du chemin
			try {
				ws_server.send(hdl, confirmation.dump(), websocketpp::frame::opcode::text);
			}
			catch (const std::exception& e) {
				std::cerr << "Erreur lors de la confirmation du chemin trouvé " << e.what() << std::endl;
			}

			// Construire le chemin
			while (!tmp.empty()) {
				path.push_back(tmp);
				tmp = previous[tmp];
			}

			// Inverser le chemin
			std::reverse(path.begin(), path.end());

			json pathNodes = json::object();

			for (int i = 1; i < path.size()-1; i++) {
				subPath.push_back(path[i]);

				// Mettre à jour les nœuds visités
				pathNodes["pathNodes"] = subPath;

				// Envoyer les nœuds visités au client via WebSocket
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des nœuds du chemin : " << e.what() << std::endl;
				}
			}

			break;
		}


		// Mettre à jour les distances pour les voisins du nœud
		for (const auto& successor : list[minNode]) {

			if (!visited[successor.first]) {
				//int minDist = distances[minNode] + successor.second;
				int minDist = distances[minNode] + 1;
				if (minDist < distances[successor.first]) {
					distances[successor.first] = minDist;
					previous[successor.first] = minNode;
				}
			}
		}

		// Envoyer les nœuds visités au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
		}
	}
}





void Algorithms::bfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end ) const
{
	// Récupérer la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// Vérifier si le graphe ou le nœud de départ sont vides
	if (list.empty() || start.empty())
	{
		std::cerr << "Graphe ou le nœud de départ vide" << std::endl;
		return;
	}

	// Vérifier si le nœud de départ existe dans le graphe
	if (list.find(start) == list.end())
	{
		std::cerr << "Le nœud '" << start << "' n'existe pas !!!" << std::endl;
		return;
	}


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


		// verifier si le chemin a été trouvé
		if (!end.empty() && end == currentNode) {
			vector<std::string> path, subPath;
			std::string tmp = currentNode;

			json confirmation = json::object();
			confirmation["pathFound"] = "true";

			// Envoyer lca confirmation de la fin du chemin
			try {
				ws_server.send(hdl, confirmation.dump(), websocketpp::frame::opcode::text);
			}
			catch (const std::exception& e) {
				std::cerr << "Erreur lors de la confirmation du chemin trouvé " << e.what() << std::endl;
			}

			// Construire le chemin
			while (!tmp.empty()) {
				path.push_back(tmp);
				tmp = parent[tmp];
			}

			// Inverser le chemin
			std::reverse(path.begin(), path.end());

			json pathNodes = json::object();

			for (int i = 1; i < path.size() - 1; i++) {
				subPath.push_back(path[i]);

				// Mettre à jour les nœuds visités
				pathNodes["pathNodes"] = subPath;

				// Envoyer les nœuds qui se trouvent dans le chemin
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
				}
			}

			break;
		}


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

	}
}



void Algorithms::dfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end ) const {
	// Récupération de la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// Vérification si le graphe ou le nœud de départ est vide
	if (list.empty() || start.empty()) {
		std::cerr << "Graphe ou le noeud de depart vide" << std::endl;
		return;
	}

	// Vérification si le nœud de départ existe dans le graphe
	if (list.find(start) == list.end()) {
		std::cerr << "Le noeud '" << start << "' n'existe pas!!!" << std::endl;
		return;
	}

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

		// si le chemin a ate trouvé
		if (!end.empty() && end == currentNode) {
			vector<std::string> path, subPath;
			std::string tmp = currentNode;

			json confirmation = json::object();
			confirmation["pathFound"] = "true";

			// Envoyer lca confirmation de la fin du chemin
			try {
				ws_server.send(hdl, confirmation.dump(), websocketpp::frame::opcode::text);
			}
			catch (const std::exception& e) {
				std::cerr << "Erreur lors de la confirmation du chemin trouvé " << e.what() << std::endl;
			}

			// Construire le chemin
			while (!tmp.empty()) {
				path.push_back(tmp);
				tmp = parent[tmp];
			}

			// Inverser le chemin
			std::reverse(path.begin(), path.end());

			json pathNodes = json::object();

			for (int i = 1; i < path.size() - 1; i++) {
				subPath.push_back(path[i]);

				// Mettre à jour les nœuds visités
				pathNodes["pathNodes"] = subPath;

				// Envoyer les nœuds visités au client via WebSocket
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
				}
			}

			break;
		}


		// Exploration des voisins du nœud courant
		for (auto elem : list[currentNode]) {
			if (colors[elem.first] == "blanc") {
				colors[elem.first] = "gris";         // Marque le voisin comme gris
				parent[elem.first] = currentNode;   // Définit le nœud courant comme parent
				Stack.push(elem.first);             // Ajoute le voisin à la pile
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
	}
}


void Algorithms::a_star(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& goal) const {
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// Vérification si le graphe ou le nœud de départ est vide
	if (list.empty() || start.empty()) {
		std::cerr << "Graphe ou le noeud de depart vide" << std::endl;
		return;
	}

	// Vérification si le nœud de départ existe dans le graphe
	if ((list.find(start) == list.end())|| list.find(goal) == list.end()) {
		std::cerr << "L'un des noeuds n'existe pas dans le graph!!!" << std::endl;
		return;
	}

	// Pour stocker le nœud précédent sur le chemin optimal
	unordered_map<string, string> previous;
	// Pour stocker les coûts cumulés depuis le point de départ jusqu'à chaque nœud
	unordered_map<string, float> g_score;
	// Pour stocker la somme de g_score et de l'estimation heuristique jusqu'au but
	unordered_map<string, float> f_score;

	// Initialisation des scores à l'infini pour chaque nœud
	for (const auto& pair : list) {
		g_score[pair.first] = numeric_limits<float>::infinity();
		f_score[pair.first] = numeric_limits<float>::infinity();
	}
	// Le coût pour atteindre le nœud de départ est de zéro
	g_score[start] = 0;
	// Le score f du nœud de départ est uniquement la valeur heuristique
	f_score[start] = heuristic(start, goal);

	// Comparateur lambda pour ordonner les nœuds selon leur f_score
	auto compare = [&f_score](const string& node1, const string& node2) {
		return f_score[node1] > f_score[node2];
		};

	// File de priorité pour stocker les nœuds à explorer
	priority_queue<string, vector<string>, decltype(compare)> open_set(compare);
	unordered_set<string> open_set_lookup; // Ensemble pour vérifier rapidement si un nœud est dans open_set

	// Ajout du nœud de départ à open_set
	open_set.push(start);
	open_set_lookup.insert(start);


	json visitedNodesJson = json::array();  // Initialisation du tableau JSON

	// Boucle principale de l'algorithme A*
	while (!open_set.empty()) {
		string current = open_set.top();
		open_set.pop();
		open_set_lookup.erase(current);

		// Si le nœud courant est le but, reconstruire le chemin
		if (current == goal) {
			json confirmation = json::object();
			confirmation["pathFound"] = "true";

			// Envoyer lca confirmation de la fin du chemin
			try {
				ws_server.send(hdl, confirmation.dump(), websocketpp::frame::opcode::text);
			}
			catch (const std::exception& e) {
				std::cerr << "Erreur lors de la confirmation du chemin trouvé " << e.what() << std::endl;
			}

			vector<string> path;
			for (string at = goal; !at.empty(); at = previous[at]) {
				path.push_back(at);
			}
			reverse(path.begin(), path.end());
			vector<std::string> subPath;
			json pathNodes = json::object();
			for (auto it = path.begin() + 1; it != path.end() - 1; ++it) {
				subPath.push_back(*it);
				// Mettre à jour les nœuds visités
				pathNodes["pathNodes"] = subPath;

				// Envoyer les nœuds visités au client via WebSocket
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
				}
			}
			return;
		}

		// ajouter le noeud dans la liste des noeuds visités
		visitedNodesJson.push_back(current);
		// Envoyer les nœuds visités au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
		}
		
		// Parcourir les voisins du nœud courant
		for (const auto& neighbor : list[current]) {
			const string& neighbor_name = neighbor.first;
			float tentative_g_score = g_score[current] + neighbor.second;

			// Si un chemin plus court est trouvé vers le voisin
			if (tentative_g_score < g_score[neighbor_name]) {
				previous[neighbor_name] = current;
				g_score[neighbor_name] = tentative_g_score;
				f_score[neighbor_name] = tentative_g_score + heuristic(neighbor_name, goal);

				// Ajouter le voisin à open_set s'il n'y est pas déjà
				if (open_set_lookup.find(neighbor_name) == open_set_lookup.end()) {
					open_set.push(neighbor_name);
					open_set_lookup.insert(neighbor_name);
				}
			}
		}
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

// Définition de la méthode heuristique de la classe Algorithms 
float Algorithms::heuristic(const std::string& node, const std::string& goal) const { 
	std::vector<int> numbers; 
	std::stringstream ss(node); 
	std::string item; 
	// Lire les coordonnées à partir de la chaîne 'node' et les convertir en entier 
	while (getline(ss, item, ',')) { 
		int num = std::stoi(item); 
		numbers.push_back(num); 
	} 
	std::stringstream ss2(goal); 
	// Lire les coordonnées à partir de la chaîne 'goal' et les convertir en entier 
	while (getline(ss2, item, ',')) { 
		int num = std::stoi(item); 
		numbers.push_back(num); 
	} 
	// Calculer la distance euclidienne entre les deux points 
	return sqrt(pow(numbers[0] - numbers[2], 2) + pow(numbers[1] - numbers[3], 2)); }

