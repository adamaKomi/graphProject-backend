#pragma once
// pour manipuler les données JSON
#include "Algorithms.h"



// Créer un graph et un objet de calcul d'algorithmes
Graph graph;
Algorithms algo;


// Gestionnaire de message WebSocket
void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg, server& s) {
    try {
        auto data = json::parse(msg->get_payload());

        if (data.find("graph") != data.end()) {
            auto graph_json = data["graph"];
            crow::json::rvalue graph_value = crow::json::load(graph_json.dump());
            graph.extractGraphData(graph_value);  
        }
       else if ((data.find("algorithm") != data.end()) && (data.find("points") != data.end())) {
            auto algorithm = data["algorithm"];
            auto points = data["points"];
            std::string point_depart = points[0].get<string>();
            std::string point_arrive = points[1].get<string>();
            std::cout << "Points depart 2 : "<< point_depart << std::endl;
            std::cout << "Points fin 2 : " << point_arrive << std::endl;
            if (algorithm == "DIJKSTRA") {
                std::cout << "(Dijkstra)..." << std::endl;
                algo.dijkstra(graph, point_depart, s, hdl);
            }else if (algorithm == "BFS") {
                std::cout << "(BFS)..." << std::endl;
                algo.bfs(graph, point_depart, s, hdl);
            }else if (algorithm == "DFS") {
                std::cout << "(DFS)..." << std::endl;
                algo.dfs(graph, point_depart, s, hdl);
            }
        }
        
        else {
            json errorResponse = {
                {"error", "Données de graphe manquantes ou invalides"}
            };
            s.send(hdl, errorResponse.dump(), websocketpp::frame::opcode::text);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur lors du traitement du message WebSocket : " << e.what() << std::endl;
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
