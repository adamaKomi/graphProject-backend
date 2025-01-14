#pragma once
#include "Algorithms.h"



void Algorithms::dijkstra(Graph& graph, const std::string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end) const {
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
	json visitedNodesJson = json::array();  // Initialisation du tableau JSON
	// Effectuer l'algorithme de Dijkstra

	while (true) {
		std::string minNode = getMinDistanceNode(list, distances, visited);
		if (minNode.empty()) break;

		visited[minNode] = true;  // Marquer le n�ud comme visit�
		visitedNodesJson.push_back(minNode);  // Ajouter le n�ud visit� au JSON

		// verifier si le chemin est trouv�
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
				std::cerr << "Erreur lors de la confirmation du chemin trouv� " << e.what() << std::endl;
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

				// Mettre � jour les n�uds visit�s
				pathNodes["pathNodes"] = subPath;

				// Envoyer les n�uds visit�s au client via WebSocket
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des n�uds du chemin : " << e.what() << std::endl;
				}
			}

			break;
		}


		// Mettre � jour les distances pour les voisins du n�ud
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

		// Envoyer les n�uds visit�s au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
		}
	}
}





void Algorithms::bfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end ) const
{
	// R�cup�rer la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// V�rifier si le graphe ou le n�ud de d�part sont vides
	if (list.empty() || start.empty())
	{
		std::cerr << "Graphe ou le n�ud de d�part vide" << std::endl;
		return;
	}

	// V�rifier si le n�ud de d�part existe dans le graphe
	if (list.find(start) == list.end())
	{
		std::cerr << "Le n�ud '" << start << "' n'existe pas !!!" << std::endl;
		return;
	}


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


		// verifier si le chemin a �t� trouv�
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
				std::cerr << "Erreur lors de la confirmation du chemin trouv� " << e.what() << std::endl;
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

				// Mettre � jour les n�uds visit�s
				pathNodes["pathNodes"] = subPath;

				// Envoyer les n�uds qui se trouvent dans le chemin
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
				}
			}

			break;
		}


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

	}
}



void Algorithms::dfs(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& end ) const {
	// R�cup�ration de la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// V�rification si le graphe ou le n�ud de d�part est vide
	if (list.empty() || start.empty()) {
		std::cerr << "Graphe ou le noeud de depart vide" << std::endl;
		return;
	}

	// V�rification si le n�ud de d�part existe dans le graphe
	if (list.find(start) == list.end()) {
		std::cerr << "Le noeud '" << start << "' n'existe pas!!!" << std::endl;
		return;
	}

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

		// si le chemin a ate trouv�
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
				std::cerr << "Erreur lors de la confirmation du chemin trouv� " << e.what() << std::endl;
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

				// Mettre � jour les n�uds visit�s
				pathNodes["pathNodes"] = subPath;

				// Envoyer les n�uds visit�s au client via WebSocket
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
				}
			}

			break;
		}


		// Exploration des voisins du n�ud courant
		for (auto elem : list[currentNode]) {
			if (colors[elem.first] == "blanc") {
				colors[elem.first] = "gris";         // Marque le voisin comme gris
				parent[elem.first] = currentNode;   // D�finit le n�ud courant comme parent
				Stack.push(elem.first);             // Ajoute le voisin � la pile
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
	}
}


void Algorithms::a_star(const Graph& graph, const string& start, server& ws_server, websocketpp::connection_hdl hdl, const string& goal) const {
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// V�rification si le graphe ou le n�ud de d�part est vide
	if (list.empty() || start.empty()) {
		std::cerr << "Graphe ou le noeud de depart vide" << std::endl;
		return;
	}

	// V�rification si le n�ud de d�part existe dans le graphe
	if ((list.find(start) == list.end())|| list.find(goal) == list.end()) {
		std::cerr << "L'un des noeuds n'existe pas dans le graph!!!" << std::endl;
		return;
	}

	// Pour stocker le n�ud pr�c�dent sur le chemin optimal
	unordered_map<string, string> previous;
	// Pour stocker les co�ts cumul�s depuis le point de d�part jusqu'� chaque n�ud
	unordered_map<string, float> g_score;
	// Pour stocker la somme de g_score et de l'estimation heuristique jusqu'au but
	unordered_map<string, float> f_score;

	// Initialisation des scores � l'infini pour chaque n�ud
	for (const auto& pair : list) {
		g_score[pair.first] = numeric_limits<float>::infinity();
		f_score[pair.first] = numeric_limits<float>::infinity();
	}
	// Le co�t pour atteindre le n�ud de d�part est de z�ro
	g_score[start] = 0;
	// Le score f du n�ud de d�part est uniquement la valeur heuristique
	f_score[start] = heuristic(start, goal);

	// Comparateur lambda pour ordonner les n�uds selon leur f_score
	auto compare = [&f_score](const string& node1, const string& node2) {
		return f_score[node1] > f_score[node2];
		};

	// File de priorit� pour stocker les n�uds � explorer
	priority_queue<string, vector<string>, decltype(compare)> open_set(compare);
	unordered_set<string> open_set_lookup; // Ensemble pour v�rifier rapidement si un n�ud est dans open_set

	// Ajout du n�ud de d�part � open_set
	open_set.push(start);
	open_set_lookup.insert(start);


	json visitedNodesJson = json::array();  // Initialisation du tableau JSON

	// Boucle principale de l'algorithme A*
	while (!open_set.empty()) {
		string current = open_set.top();
		open_set.pop();
		open_set_lookup.erase(current);

		// Si le n�ud courant est le but, reconstruire le chemin
		if (current == goal) {
			json confirmation = json::object();
			confirmation["pathFound"] = "true";

			// Envoyer lca confirmation de la fin du chemin
			try {
				ws_server.send(hdl, confirmation.dump(), websocketpp::frame::opcode::text);
			}
			catch (const std::exception& e) {
				std::cerr << "Erreur lors de la confirmation du chemin trouv� " << e.what() << std::endl;
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
				// Mettre � jour les n�uds visit�s
				pathNodes["pathNodes"] = subPath;

				// Envoyer les n�uds visit�s au client via WebSocket
				try {
					ws_server.send(hdl, pathNodes.dump(), websocketpp::frame::opcode::text);
				}
				catch (const std::exception& e) {
					std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
				}
			}
			return;
		}

		// ajouter le noeud dans la liste des noeuds visit�s
		visitedNodesJson.push_back(current);
		// Envoyer les n�uds visit�s au client via WebSocket
		try {
			ws_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) {
			std::cerr << "Erreur lors de l'envoi des n�uds visit�s : " << e.what() << std::endl;
		}
		
		// Parcourir les voisins du n�ud courant
		for (const auto& neighbor : list[current]) {
			const string& neighbor_name = neighbor.first;
			float tentative_g_score = g_score[current] + neighbor.second;

			// Si un chemin plus court est trouv� vers le voisin
			if (tentative_g_score < g_score[neighbor_name]) {
				previous[neighbor_name] = current;
				g_score[neighbor_name] = tentative_g_score;
				f_score[neighbor_name] = tentative_g_score + heuristic(neighbor_name, goal);

				// Ajouter le voisin � open_set s'il n'y est pas d�j�
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

// D�finition de la m�thode heuristique de la classe Algorithms 
float Algorithms::heuristic(const std::string& node, const std::string& goal) const { 
	std::vector<int> numbers; 
	std::stringstream ss(node); 
	std::string item; 
	// Lire les coordonn�es � partir de la cha�ne 'node' et les convertir en entier 
	while (getline(ss, item, ',')) { 
		int num = std::stoi(item); 
		numbers.push_back(num); 
	} 
	std::stringstream ss2(goal); 
	// Lire les coordonn�es � partir de la cha�ne 'goal' et les convertir en entier 
	while (getline(ss2, item, ',')) { 
		int num = std::stoi(item); 
		numbers.push_back(num); 
	} 
	// Calculer la distance euclidienne entre les deux points 
	return sqrt(pow(numbers[0] - numbers[2], 2) + pow(numbers[1] - numbers[3], 2)); }

