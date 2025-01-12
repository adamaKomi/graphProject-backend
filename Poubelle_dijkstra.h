#pragma once


/*







void Algorithms::dijkstra(Graph& graph, const std::string& start) const {
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
	std::cout << "Hello 1" << std::endl;
	std::unordered_map<std::string, int> distances;
	std::unordered_map<std::string, bool> visited;
	std::unordered_map<std::string, std::string> previous;

	// Initialiser les distances et les nœuds visités
	for (const auto& elem : list) {
		distances[elem.first] = INT_MAX;
		visited[elem.first] = false;
	}
	std::cout << "Hello 2" << std::endl;
	distances[start] = 0;
	previous[start] = "";

	// Initialisation du serveur WebSocket
	server print_server;
	std::cout << "Hello 3" << std::endl;
	// Set handler for open event
	print_server.set_open_handler([&](websocketpp::connection_hdl hdl) {
		std::cout << "Hello 4" << std::endl;
		json visitedNodesJson = json::array();  // Initialisation du tableau JSON
		std::cout << "Hello 5" << std::endl;
		// Effectuer l'algorithme de Dijkstra
		while (true) {
			std::cout << "Hello A";
			std::string minNode = getMinDistanceNode(list, distances, visited);
			if (minNode.empty()) break;

			visited[minNode] = true;  // Marquer le nœud comme visité
			visitedNodesJson.push_back(minNode);  // Ajouter le nœud visité au JSON

			// Mettre à jour les distances pour les voisins du nœud
			for (const auto& successor : list[minNode]) {
				std::cout << "Hello B" ;
				if (!visited[successor.first]) {
					int minDist = distances[minNode] + successor.second;
					if (minDist < distances[successor.first]) {
						distances[successor.first] = minDist;
						previous[successor.first] = minNode;
					}
				}
			}

			// Envoyer les nœuds visités à chaque itération au client
			try {
				std::cout << "Hello 6" << std::endl;
				// Appel à la fonction d'envoi de nœuds visités
				send_visited_nodes({ minNode }, print_server);
				std::cout << "Hello 7" << std::endl;
			}
			catch (const std::exception& e) {
				std::cerr << "Erreur lors de l'envoi des nœuds visités : " << e.what() << std::endl;
				std::cout << "Hello 8" << std::endl;
			}
		}
		});

	std::cout << "Hello 9" << std::endl;
	// Start WebSocket server
	print_server.listen(3002);
	print_server.start_accept();
	print_server.run();
	std::cout << "Hello 10" << std::endl;
}












#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "Algorithms.h"
#include "Graph.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
typedef websocketpp::server<websocketpp::config::asio> server;
using json = nlohmann::json;

void Algorithms::dijkstra(Graph& graph, const std::string& start) const {
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

	std::unordered_map<std::string, int> distances;
	std::unordered_map<std::string, bool> visited;
	std::unordered_map<std::string, std::string> previous;
	std::wcout << "hello 1" << std::endl;

	for (const auto& elem : list) {
		distances[elem.first] = INT_MAX;
		visited[elem.first] = false;
	}
	std::wcout << "hello 2" << std::endl;

	distances[start] = 0;
	previous[start] = "";

	server print_server;

	try {
		print_server.init_asio();
		std::wcout << "Initializing WebSocket server..." << std::endl;
	}
	catch (const std::exception& e) {
		std::wcerr << L"Error initializing ASIO: " << e.what() << std::endl;
		return;
	}

	// Set handler for open event
	print_server.set_open_handler([&](websocketpp::connection_hdl hdl) {
		std::wcout << "hello 4" << std::endl;
		json visitedNodesJson = json::array();
		std::wcout << "hello 5" << std::endl;

		while (true) {
			std::wcout << "hello 6" << std::endl;
			std::string minNode = getMinDistanceNode(list, distances, visited);
			if (minNode.empty()) break;
			visited[minNode] = true;
			visitedNodesJson.push_back(minNode);
			std::wcout << "hello 7" << std::endl;

			for (const auto& successor : list[minNode]) {
				if (!visited[successor.first]) {
					int minDist = distances[minNode] + successor.second;
					if (minDist < distances[successor.first]) {
						distances[successor.first] = minDist;
						previous[successor.first] = minNode;
					}
				}
			}

			try {
				print_server.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
			}
			catch (const std::exception& e) {
				std::wcerr << L"Error sending data: " << e.what() << std::endl;
				break;
			}
		}
		});

	// Listen on port 3003 and start accepting connections
	try {
		print_server.listen(3003);
		std::wcout << "Server is listening on port 3003..." << std::endl;
	}
	catch (const std::exception& e) {
		std::wcerr << L"Error while listening on port 3003: " << e.what() << std::endl;
		return;
	}

	try {
		print_server.start_accept();
		std::wcout << "Server is accepting connections..." << std::endl;
	}
	catch (const std::exception& e) {
		std::wcerr << L"Error during start_accept: " << e.what() << std::endl;
		return;
	}

	// Run the server
	try {
		print_server.run();
		std::wcout << "Server is running..." << std::endl;
	}
	catch (const std::exception& e) {
		std::wcerr << L"Error while running the server: " << e.what() << std::endl;
		return;
	}

	std::wcout << "hello 9" << std::endl;
}












void Algorithms::dijkstra(Graph& graph, const string& start) const
{
	// Récupérer la liste d'adjacence du graphe
	unordered_map<string, vector<pair<string, int>>> list = graph.getList();

	// Vérifier si le graphe ou le nœud de départ ne sont pas vides
	if (list.empty() || start.empty()) {
		std::cout << "Graphe ou le nœud de départ vide" << std::endl;
		return;
	}

	// Vérifier si le nœud de départ existe dans le graphe
	if (list.find(start) == list.end()) {
		std::cout << "Le nœud '" << start << "' n'existe pas !!!" << std::endl;
		return;
	}

	std::cout << std::endl;
	std::cout << "================================================================= " << std::endl;
	std::cout << "---------------------ALGORITHME DE DIJKSTRA---------------------" << std::endl;
	std::cout << "Sommet de départ : " << start << std::endl;
	std::cout << "================================================================= " << std::endl;

	// Les structures de données
unordered_map<string, int> distances; // Pour les distances minimales
unordered_map<string, bool> visited;  // Pour marquer les nœuds déjà visités
unordered_map<string, string> previous; // Pour enregistrer le prédécesseur d'un nœud

// Initialisation des structures
for (auto elem : list) {
	distances[elem.first] = INT_MAX;
	visited[elem.first] = false;
}

// Initialiser le nœud de départ
distances[start] = 0;
previous[start] = "";

// Boucle principale
while (true)
{
	// Récupérer le nœud de plus petite distance et non visité
	string minNode = getMinDistanceNode(list, distances, visited);

	// Arrêter le traitement si tous les nœuds ont été visités
	if (minNode.empty()) break;

	// Marquer le nœud trouvé comme visité
	visited[minNode] = true;

	// Récupérer les successeurs du nœud 
	vector<pair<string, int>> successors = list[minNode];

	// Traitement des successeurs
	for (auto successor : successors)
	{
		// Si le successeur n'a pas été visité, on calcule la nouvelle distance
		if (!visited[successor.first])
		{
			int minDist = distances.at(minNode) + successor.second;

			// Si la nouvelle distance est inférieure à la distance courante, on met à jour
			if (minDist < distances.at(successor.first))
			{
				distances.at(successor.first) = minDist;
				previous[successor.first] = minNode; // Sauvegarder le prédécesseur
			}
		}
	}

	// Afficher l'état actuel des distances
	//std::cout << "Distances :\n";
	//for (const auto& pair : distances)
	//{
	//	std::cout << pair.first << " : ";
	//	if (pair.second == INT_MAX)
	//		std::cout << "INF";
	//	else
	//		std::cout << pair.second;
	//	std::cout << (visited[pair.first] ? " (visité)" : " (non visité)") << std::endl;
	//}


	// Préparer les données JSON des nœuds visités
	json visitedNodesJson = json::array();
	for (const auto& entry : visited) {
		if (entry.second) {
			visitedNodesJson.push_back(entry.first);
		}
	}

	// Envoyer les données à l'API Flask
	httplib::Client cli("http://localhost:5000");
	auto res = cli.Post("/visitedNodes", visitedNodesJson.dump(), "application/json");

	if (res && res->status == 200) {
		std::cout << "Data sent successfully to Flask API." << std::endl;
	}
	else {
		std::cout << "Failed to send data to Flask API." << std::endl;
	}

}

// Affichage des chemins à la fin
//std::cout << "\n=================================================================" << std::endl;
//std::cout << "Chemins trouvés depuis le sommet de départ '" << start << "':" << std::endl;
//std::cout << "=================================================================\n";

//for (const auto& pair : distances) {
//	if (pair.second != INT_MAX) {
//		// Reconstituer le chemin pour chaque nœud atteint
//		string currentNode = pair.first;
//		std::vector<string> path;

//		while (!currentNode.empty()) {
//			path.push_back(currentNode);
//			currentNode = previous[currentNode];
//		}

//		// Afficher le chemin (inverser le vecteur pour partir du début)
//		//std::reverse(path.begin(), path.end());
//		//std::cout << "Chemin vers " << pair.first << " [Distance: " << pair.second << "] : ";
//		//for (size_t i = 0; i < path.size(); ++i) {
//		//	std::cout << path[i];
//		//	if (i < path.size() - 1) std::cout << " -> ";
//		//}
//		std::cout << std::endl;
//	}
//}
}

































*/