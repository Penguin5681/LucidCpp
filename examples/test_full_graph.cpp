#include <iostream>
#include <string>
#include <vector>
#include "vis/Vis.hpp" 

struct GraphNode {
    std::string val;
    std::vector<std::pair<GraphNode*, int>> edges;
    
    GraphNode(std::string v) : val(v) {}
};

struct FullGraph {
    std::vector<GraphNode*> all_nodes;
};

int main() {
    std::cout << "[Test 6] Full Graph Object...\n";
    GraphNode* a = new GraphNode{"Node A"};
    GraphNode* b = new GraphNode{"Node B"};
    GraphNode* c = new GraphNode{"Node C"};
    GraphNode* d = new GraphNode{"Node D (Disconnected)"};

    a->edges.push_back({b, 10});
    b->edges.push_back({a, 10});
    
    b->edges.push_back({c, 5});
    c->edges.push_back({b, 5});

    FullGraph myGraph;
    myGraph.all_nodes = {a, b, c, d};

    vis::writeGraphHTMLFile(myGraph, "test6_full_graph.html", 
        [](FullGraph& g) { return g.all_nodes; },
        [](GraphNode* n) { return n->val; },
        [](GraphNode* n) { return n->edges; }
    );

    return 0;
}
