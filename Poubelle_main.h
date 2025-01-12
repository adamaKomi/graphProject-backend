#pragma once
/*







#pragma once
// pour manipuler les données JSON
#include "Algorithms.h"



// Créer un graph et un objet de calcul d'algorithmes
Graph graph;
Algorithms algo;


// Gestionnaire de message WebSocket
void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg, server& s) {
    try {
        // Charger les données JSON envoyées par le client
        auto data = json::parse(msg->get_payload());

        // Vérifier si les données contiennent un graphe valide
        if (data.find("graph") != data.end()) {
            // Récupérer la valeur de la clé 'graph' et la convertir en crow::json::rvalue
            auto graph_json = data["graph"];
            crow::json::rvalue graph_value = crow::json::load(graph_json.dump());

            // Extraire et traiter les données de graphe envoyées par le client
            std::cout << "Extraction : " << std::endl;
            graph.extractGraphData(graph_value);

            std::cout << "Calcul du plus court chemin (Dijkstra)..." << std::endl;

            // Appliquer l'algorithme Dijkstra
            algo.dijkstra(graph, "0,1");


        }
        else {
            // Si les données reçues ne sont pas valides ou ne contiennent pas de graphe
            json errorResponse = {
                {"error", "Données de graphe manquantes ou invalides"}
            };
            s.send(hdl, errorResponse.dump(), websocketpp::frame::opcode::text);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors du traitement du message WebSocket : " << e.what() << std::endl;

        // Envoi d'un message d'erreur si une exception se produit
        json errorResponse = {
            {"error", "Erreur lors du traitement des données"}
        };
        s.send(hdl, errorResponse.dump(), websocketpp::frame::opcode::text);
    }
}

// Fonction pour gérer l'ajout de nouvelles connexions
void on_open(websocketpp::connection_hdl hdl, server& s) {
    connected_clients.insert(hdl);  // Ajouter la connexion à la liste des clients connectés
}

// Fonction pour gérer les fermetures de connexion
void on_close(websocketpp::connection_hdl hdl, server& s) {
    connected_clients.erase(hdl);  // Retirer la connexion de la liste des clients connectés
}

// Fonction pour démarrer le serveur WebSocket
void start_websocket_server() {
    server ws_server;

    try {
        // Initialiser le serveur WebSocket
        ws_server.init_asio();

        // Lier les gestionnaires d'événements
        ws_server.set_open_handler([&](websocketpp::connection_hdl hdl) {
            on_open(hdl, ws_server);
            });
        ws_server.set_close_handler([&](websocketpp::connection_hdl hdl) {
            on_close(hdl, ws_server);
            });

        ws_server.set_message_handler([&](websocketpp::connection_hdl hdl, server::message_ptr msg) {
            on_message(hdl, msg, ws_server);
            });

        // Démarrer le serveur WebSocket pour écouter sur le port 3002
        ws_server.listen(3002);
        ws_server.start_accept();

        std::cout << "Serveur WebSocket démarré sur ws://localhost:3002" << std::endl;

        // Exécuter le serveur pour accepter les connexions
        ws_server.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors du démarrage du serveur WebSocket : " << e.what() << std::endl;
    }
}

int main() {
    // Démarrer le serveur WebSocket
    start_websocket_server();
    return 0;
}














#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "Algorithms.h"
#include "Graph.h"
#include "nlohmann/json.hpp"  // pour manipuler les données JSON

using json = nlohmann::json;
typedef websocketpp::server<websocketpp::config::asio> server;

// Créer un graph
Graph graph;
Algorithms algo;

// Fonction pour envoyer des messages via WebSocket
void send_visited_nodes(websocketpp::connection_hdl hdl, server& s, const std::vector<std::string>& visitedNodes) {
    json visitedNodesJson = json::array();
    for (const auto& node : visitedNodes) {
        visitedNodesJson.push_back(node);
    }
    s.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
}

// Gestionnaire de message WebSocket
void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg, server& s) {
    try {
        // Charger les données JSON envoyées par le client
        auto data = json::parse(msg->get_payload());

        // Vérifier si les données contiennent un graphe valide
        if (data.find("graph") != data.end()) {
            // Récupérer la valeur de la clé 'graph' et la convertir en crow::json::rvalue
            auto graph_json = data["graph"];
            crow::json::rvalue graph_value = crow::json::load(graph_json.dump());

            //std::cout << "Donnees graph recues : "<<graph_value << std::endl;
            // Extraire et traiter les données de graphe envoyées par le client
            std::cout << "Extraction : " << std::endl;
            graph.extractGraphData(graph_value);
            //std::cout << "Affichage du graph : " << std::endl;
            //graph.printGraph();

            std::cout << "Calcul du plus court chemin (Dijkstra)..." << std::endl;

            // //Appliquer l'algorithme Dijkstra
            algo.dijkstra(graph, "0,1");

            // Simuler les nœuds visités (remplacer cela par vos données réelles)
            //std::vector<std::string> visitedNodes = { "0,1", "0,2", "1,3" };
            //send_visited_nodes(hdl, s, visitedNodes);
        }
        else {
            // Si les données reçues ne sont pas valides ou ne contiennent pas de graphe
            json errorResponse = {
                {"error", "Données de graphe manquantes ou invalides"}
            };
            s.send(hdl, errorResponse.dump(), websocketpp::frame::opcode::text);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors du traitement du message WebSocket : " << e.what() << std::endl;

        // Envoi d'un message d'erreur si une exception se produit
        json errorResponse = {
            {"error", "Erreur lors du traitement des données"}
        };
        s.send(hdl, errorResponse.dump(), websocketpp::frame::opcode::text);
    }
}

// Fonction pour démarrer le serveur WebSocket
void start_websocket_server() {
    server ws_server;

    try {
        // Initialiser le serveur WebSocket
        ws_server.init_asio();

        // Lier les gestionnaires d'événements
        ws_server.set_message_handler([&](websocketpp::connection_hdl hdl, server::message_ptr msg) {
            on_message(hdl, msg, ws_server);
            });

        // Démarrer le serveur WebSocket pour écouter sur le port 3002
        ws_server.listen(3002);
        ws_server.start_accept();

        std::cout << "Serveur WebSocket démarré sur ws://localhost:3002" << std::endl;

        // Exécuter le serveur pour accepter les connexions
        ws_server.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors du démarrage du serveur WebSocket : " << e.what() << std::endl;
    }
}

int main() {
    // Démarrer le serveur WebSocket
    start_websocket_server();
    return 0;
}




*/