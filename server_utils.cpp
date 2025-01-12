#include "server_utils.h"


// D�finition de la variable
std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connected_clients;

void send_visited_nodes(const std::vector<std::string>& visitedNodes, server& s) {
    json visitedNodesJson = json::array();
    for (const auto& node : visitedNodes) {
        visitedNodesJson.push_back(node);
    }

    // Envoi � tous les clients connect�s
    for (const auto& hdl : connected_clients) {
        s.send(hdl, visitedNodesJson.dump(), websocketpp::frame::opcode::text);
    }
}
