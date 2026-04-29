#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include "vis/Vis.hpp"

class Graph {
public: 
    std::unordered_map<int, std::vector<int>> adjList;

    void addEdge(int u, int v) 
    {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    void BFSTraversal(int startingNode) 
    {
        std::unordered_set<int> visitedNodes;
        std::queue<int> q;

        visitedNodes.insert(startingNode);
        q.push(startingNode);

        std::cout << "BFS Traversal" << std::endl;

        while (!q.empty())
        {
            int currentNode = q.front();
            q.pop();

            std::cout << currentNode << " ";

            for (const auto neighbourNode : adjList[currentNode])
            {
                if (visitedNodes.find(neighbourNode) == visitedNodes.end())
                {
                    visitedNodes.insert(neighbourNode);
                    q.push(neighbourNode);
                }
            }
        }

        std::cout << std::endl;
    }
};

Graph buildRandomGraph(int nodeCount, double probability) 
{
    Graph randomGraph;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    // std loop
    for (int i = 0; i < nodeCount; i++) {
        for (int j = i + 1; j < nodeCount; j++) {
            if (dis(gen) < probability) {
                randomGraph.addEdge(i, j);
            }
        }
    }

    return randomGraph;
}

int main() {
    
    Graph someGraph = buildRandomGraph(6, 0.6);

    someGraph.BFSTraversal(0);

    vis::writeGraphHTMLFile(someGraph, "random_graph.html",
        [](Graph& g) { 
            std::vector<int> nodes;
            for (const auto& pair : g.adjList) nodes.push_back(pair.first);
            return nodes;
        },
        [](int n) { return std::to_string(n); },
        [&someGraph](int n) { return someGraph.adjList[n]; }
    );

    return 0;
}
