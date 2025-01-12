#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>

#include <httplib.h>

#include "Graph.h"
#include <ctype.h>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>


// Liste des connexions WebSocket actives
// D�claration de la variable connect�e (sans d�finition)
extern std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connected_clients;


using json = nlohmann::json;
typedef websocketpp::server<websocketpp::config::asio> server;

// D�claration de la fonction d'envoi
void send_visited_nodes(const std::vector<std::string>& visitedNodes, server& s);

#endif // SERVER_UTILS_H
