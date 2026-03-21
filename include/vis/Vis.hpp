#pragma once
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <concepts>
#include <unordered_set>
#include <queue>

#include "../DSHeaders/ListNode.hpp"

// clang-format off
namespace osdetecter
{
	inline int OsType()
	{
		#if defined(_WIN32)
			return 0;
		#elif defined(__linux__)
			return 1;
		#elif defined(__APPLE__) && defined(__MACH__)
			return 2;				
		#elif defined(__unix__)
			return 3;
		#else
			return -1;
		#endif

		return 0;
	}
} // namespace osdetecter

namespace vis
{
    template <typename T>
    concept LinearNode = requires(T node) {
        { node.data };
        { node.next } -> std::convertible_to<T*>;
    };
    // TODO: Define Binary Tree Node Concept
    // TODO: Define Generic Tree Node Concept
    // TODO: Figure out the concept for graphs

	inline void exploreStructure(ListNode *rootNode)
	{
		while (rootNode != nullptr)
		{
			std::cout << rootNode->data << " -> ";
			rootNode = rootNode->next;
		}
		std::cout << "NULL";
		std::cout << std::endl;
	}

    template <typename T>
	inline std::string getNodeAddress(T *node)
	{
		std::stringstream ss;
		ss << "0x" << std::hex << reinterpret_cast<uintptr_t>(node);
		return ss.str();
	}

	inline void exploreListAddressing(ListNode *rootNode)
	{
		while (rootNode != nullptr)
		{
			std::cout << "Visting: " << getNodeAddress(rootNode)
					  << " Data: " << rootNode->data << "\n";
			rootNode = rootNode->next;
		}
		std::cout << "NULL";
		std::cout << std::endl;
	}

	inline void explore(const std::string &message)
	{
		std::cout << message << std::endl;
	}

	// NOTE: This will output a .dot file containing the renderable graphviz dot
	// graph

	inline void writeListDotFile(ListNode *rootNode,
								 const std::string &path,
								 const std::string svgOutputPath = "out.svg")
	{
		std::ofstream out(path);
		out << "digraph G {\n";
		for (auto it = rootNode; it != nullptr; it = it->next)
		{
			out << "\"" << getNodeAddress(it) << "\"";
			out << " [label=\"" << it->data << "\"];\n";
			out << "\"" << getNodeAddress(it) << "\"";
			out << " -> \"" << getNodeAddress(it->next) << "\";\n";
			out << std::endl;
		}
		out << "}\n";
		out.close();

		std::cout << "The dot file has been written to: " << path << std::endl;

		int osType = osdetecter::OsType();

		std::string command = "dot -Tsvg " + path + " -o " + svgOutputPath;
		std::system(command.c_str());
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::string openCommand = "";
		// 0 => win
		// 1 => linux
		// 2 => mac 
		// 3 => unix like
		// -1 => unidentified
		switch (osType) {
			case 0:
				openCommand += "start " + svgOutputPath;
				break;
			case 1:
				openCommand += "open " + svgOutputPath;
				break;
			case 2:
				openCommand += "xdg-open " + svgOutputPath;
				break;
			case 3:
				openCommand += "open " + svgOutputPath;
				break;
			case -1:
				openCommand += "echo Error Occured!";
				break;
			default:
				break;
		}
		std::system(openCommand.c_str());
		return;
	}

    template <typename T, typename DataFunc, typename ChildrenFunc>
    inline std::pair<std::string, std::string> getTreeEdgesJson(T* rootNode, DataFunc getData, ChildrenFunc getChildren) {
        std::unordered_set<const void*> visitedNodes;
        std::queue<T*> q;
        std::stringstream nodesJson;
        std::stringstream edgesJson;

        nodesJson << "[\n"; edgesJson << "[\n";
        bool isFirstNode = true;
        bool isFirstEdge = true;

        if (rootNode != nullptr) {
            q.push(rootNode);
            visitedNodes.insert(rootNode);
        }

        while (!q.empty()) {
            T* current = q.front();
            q.pop();

            // FIX 1: Add comma for Nodes
            if (!isFirstNode) nodesJson << ",\n";
            nodesJson << "  { \"id\": \"" << getNodeAddress(current) << "\", \"label\": \"" << getData(current) << "\" }";
            isFirstNode = false;

            std::vector<T*> children = getChildren(current);
            for (T* child : children) {
                if (child == nullptr) {
                    continue;
                }
                
                // FIX 2: Add comma for Edges
                if (!isFirstEdge) edgesJson << ",\n";
                edgesJson << "  { \"from\": \"" << getNodeAddress(current) << "\", \"to\": \"" << getNodeAddress(child) << "\" }";
                isFirstEdge = false;

                if (visitedNodes.find(child) == visitedNodes.end()) {
                    visitedNodes.insert(child);
                    q.push(child);
                }
            }
        }

        nodesJson << "\n]";
        edgesJson << "\n]";
        
        return std::make_pair(nodesJson.str(), edgesJson.str());
    }

    template <typename T, typename DataFunc, typename ChildrenFunc>
	inline std::pair<std::string, std::string> getNodesEdgesJson(T* rootNode, DataFunc getData, ChildrenFunc getChildren) {
        std::unordered_set<const void*> visitedNodes;
		std::stringstream nodesJson;
		std::stringstream edgesJson;
		
		nodesJson << "[\n";
		edgesJson << "[\n";
		
		bool isFirstNode = true;
    	bool isFirstEdge = true;
        bool isCycle = false;
		for (auto it = rootNode; it != nullptr; it = getChildren(it))
		{
            if (visitedNodes.find(it) != visitedNodes.end()) {
                isCycle = true;
                break;
            }
            if (!isFirstNode) nodesJson << ",\n";
			nodesJson << "  { \"id\": \"" << getNodeAddress(it) << "\", \"label\": \"" << getData(it) << "\" }";
			isFirstNode = false;

			if (getChildren(it) != nullptr)
			{
				if (!isFirstEdge) edgesJson << ",\n";
				edgesJson << "  { \"from\": \"" << getNodeAddress(it) << "\", \"to\": \"" << getNodeAddress(getChildren(it)) << "\" }";
				isFirstEdge = false;
			}
            visitedNodes.insert(it);
        }

		nodesJson << "\n]";
		edgesJson << "\n]"; 

        std::cout << nodesJson.str();
        std::cout << "--------------------";
        std::cout << edgesJson.str();
		
		return std::make_pair(nodesJson.str(), edgesJson.str());
	}

    inline std::string getListHtmlTemplate(const std::string& nodesJson, const std::string& edgesJson) {
        return R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Linked List Render</title>
                <style>
                    :root {
                        --bg: #f5f2ea;
                        --ink: #1b1b1b;
                        --accent: #e07a5f;
                        --panel: #fff7e8;
                        --border: #2f2f2f;
                    }
                    * { box-sizing: border-box; }
                    body {
                        margin: 0;
                        font-family: "Space Grotesk", "Trebuchet MS", sans-serif;
                        background: radial-gradient(1200px 800px at 20% 10%, #fff2d8, var(--bg));
                        color: var(--ink);
                    }
                    .header {
                        padding: 20px 24px 8px;
                    }
                    .header h1 {
                        margin: 0;
                        font-size: 20px;
                        letter-spacing: 1px;
                        text-transform: uppercase;
                    }
                    .header p {
                        margin: 6px 0 0;
                        font-size: 13px;
                        opacity: 0.75;
                    }
                    #list {
                        display: flex;
                        align-items: center;
                        gap: 18px;
                        padding: 40px 24px 32px; /* Increased top padding to make room for tooltip */
                        overflow-x: auto;
                        min-height: 200px;
                    }
                    .node {
                        position: relative;
                        display: grid;
                        grid-template-columns: 1fr 1fr;
                        min-width: 200px; /* Slightly wider to accommodate hex strings */
                        border: 2px solid var(--border);
                        background: var(--panel);
                        box-shadow: 6px 6px 0 #2f2f2f22;
                        transform: translateY(0);
                        transition: transform 160ms ease, box-shadow 160ms ease;
                    }
                    .node:hover {
                        transform: translateY(-4px);
                        box-shadow: 10px 10px 0 #2f2f2f22;
                    }
                    /* NEW: The floating tooltip for the current address */
                    .current-addr {
                        position: absolute;
                        top: -30px;
                        left: 50%;
                        transform: translateX(-50%) translateY(5px);
                        background: var(--ink);
                        color: var(--bg);
                        padding: 4px 8px;
                        border-radius: 4px;
                        font-size: 11px;
                        font-family: monospace;
                        opacity: 0;
                        pointer-events: none;
                        transition: opacity 160ms ease, transform 160ms ease;
                        white-space: nowrap;
                    }
                    /* Show tooltip on node hover */
                    .node:hover .current-addr {
                        opacity: 1;
                        transform: translateX(-50%) translateY(0);
                    }
                    .block {
                        padding: 12px 14px;
                        border-right: 2px solid var(--border);
                        min-height: 70px;
                        display: flex;
                        flex-direction: column;
                        justify-content: center;
                        max-width: 120px; /* Force a max width so it wraps */
                    }
                    .block:last-child {
                        border-right: none;
                    }
                    .label {
                        font-size: 11px;
                        letter-spacing: 1px;
                        text-transform: uppercase;
                        opacity: 0.6;
                    }
                    .value {
                        margin-top: 6px;
                        font-size: 15px;
                        font-weight: 600;
                        word-break: break-word; /* Prevents massive data from overflowing */
                    }
                    .next-value {
                        display: inline-block;
                        padding: 4px 6px;
                        border: 1px dashed var(--border);
                        background: #fff;
                        opacity: 0;
                        font-family: monospace; /* Monospace looks better for hex */
                        font-size: 11px; /* Shrink address text */
                        word-break: break-all; /* CRITICAL: forces the hex string to wrap to the next line */
                        transform: translateY(4px);
                        transition: opacity 140ms ease, transform 140ms ease;
                    }
                    .node:hover .next-value {
                        opacity: 1;
                        transform: translateY(0);
                    }
                    .arrow {
                        font-size: 20px;
                        color: var(--accent);
                    }
                    .tail {
                        font-weight: 600;
                        padding: 10px 14px;
                        border: 2px dashed var(--border);
                        background: #fff;
                    }
                </style>
            </head>
            <body>
                <div class="header">
                    <h1>Linked List</h1>
                    <p>Hover a node to reveal its memory address and next pointer.</p>
                </div>
                <div id="list"></div>
                <script type="text/javascript">
                    const nodes = )" + nodesJson + R"(;
                    const edges = )" + edgesJson + R"(;
                    
                    const nextMap = new Map();
                    for (const edge of edges) {
                        nextMap.set(edge.from, edge.to);
                    }

                    const listEl = document.getElementById('list');
                    for (let i = 0; i < nodes.length; i++) {
                        const node = nodes[i];
                        const nextAddr = nextMap.get(node.id) || 'NULL';

                        const card = document.createElement('div');
                        card.className = 'node';

                        // NEW: Create and append the current address tooltip
                        const tooltip = document.createElement('div');
                        tooltip.className = 'current-addr';
                        tooltip.textContent = 'Addr: ' + node.id;
                        card.appendChild(tooltip);

                        const dataBlock = document.createElement('div');
                        dataBlock.className = 'block';
                        dataBlock.innerHTML = '<span class="label">data</span><span class="value">' + node.label + '</span>';

                        const nextBlock = document.createElement('div');
                        nextBlock.className = 'block';
                        nextBlock.innerHTML = '<span class="label">next</span><span class="value next-value">' + nextAddr + '</span>';

                        card.appendChild(dataBlock);
                        card.appendChild(nextBlock);
                        listEl.appendChild(card);

                        if (i < nodes.length - 1) {
                            const arrow = document.createElement('div');
                            arrow.className = 'arrow';
                            arrow.textContent = '\u2192';
                            listEl.appendChild(arrow);
                        }
                    }

                    const lastNode = nodes[nodes.length - 1];
                    const finalTargetAddr = nextMap.get(lastNode.id);

                    if (finalTargetAddr) {
                        const targetNode = nodes.find(n => n.id === finalTargetAddr);
                        const targetLabel = targetNode ? targetNode.label : "Unknown";

                        const arrow = document.createElement('div');
                        arrow.className = 'arrow';
                        arrow.innerHTML = '&#8617;'; 
                        listEl.appendChild(arrow);

                        const cycleBlock = document.createElement('div');
                        cycleBlock.className = 'tail';
                        cycleBlock.style.borderColor = 'var(--accent)'; 
                        cycleBlock.style.color = 'var(--accent)';
                        cycleBlock.innerHTML = '&#128260; Cycles to: <b>' + targetLabel + '</b>';
                        listEl.appendChild(cycleBlock);
                    } else {
                        const tail = document.createElement('div');
                        tail.className = 'tail';
                        tail.textContent = 'NULL';
                        listEl.appendChild(tail);
                    }
                </script>
            </body>
            </html>
        )";
    }
	// NOTE: This will output a .html file containing the renderable content
	// opened in browser ofc

    inline std::string getTreeHtmlTemplate(const std::string& nodesJson, const std::string& edgesJson) {
        return R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Tree Render</title>
                <style>
                    :root {
                        --bg: #f5f2ea;
                        --ink: #1b1b1b;
                        --accent: #e07a5f;
                        --panel: #fff7e8;
                        --border: #2f2f2f;
                    }
                    * { box-sizing: border-box; }
                    body {
                        margin: 0;
                        font-family: "Space Grotesk", "Trebuchet MS", sans-serif;
                        background: radial-gradient(1200px 800px at 20% 10%, #fff2d8, var(--bg));
                        color: var(--ink);
                    }
                    .header {
                        padding: 20px 24px 8px;
                    }
                    .header h1 {
                        margin: 0;
                        font-size: 20px;
                        letter-spacing: 1px;
                        text-transform: uppercase;
                    }
                    .header p {
                        margin: 6px 0 0;
                        font-size: 13px;
                        opacity: 0.75;
                    }
                    #tree {
                        padding: 40px 24px;
                        overflow: auto;
                        min-height: 600px;
                        display: flex;
                        justify-content: center;
                    }
                    .tree-container {
                        display: flex;
                        flex-direction: column;
                        align-items: center;
                        gap: 40px;
                    }
                    .tree-node {
                        position: relative;
                        display: inline-block;
                        padding: 12px 16px;
                        border: 2px solid var(--border);
                        background: var(--panel);
                        box-shadow: 6px 6px 0 #2f2f2f22;
                        border-radius: 4px;
                        cursor: pointer;
                        transition: all 160ms ease;
                        max-width: 180px;
                        word-wrap: break-word;
                    }
                    .tree-node:hover {
                        transform: translateY(-4px);
                        box-shadow: 10px 10px 0 #2f2f2f22;
                        background: #fffbf0;
                    }
                    .tree-node .label {
                        font-size: 10px;
                        opacity: 0.6;
                        text-transform: uppercase;
                        letter-spacing: 1px;
                    }
                    .tree-node .value {
                        font-size: 14px;
                        font-weight: 600;
                        margin-top: 4px;
                    }
                    .tree-level {
                        display: flex;
                        justify-content: center;
                        gap: 30px;
                        flex-wrap: wrap;
                    }
                    .tree-branch {
                        position: relative;
                        display: flex;
                        flex-direction: column;
                        align-items: center;
                    }
                    svg.connector {
                        position: absolute;
                        pointer-events: none;
                    }
                </style>
            </head>
            <body>
                <div class="header">
                    <h1>Tree Structure</h1>
                    <p>Interactive visualization of tree nodes and their connections.</p>
                </div>
                <div id="tree"></div>
                <script type="text/javascript">
                    const nodes = )" + nodesJson + R"(;
                    const edges = )" + edgesJson + R"(;
                    
                    // Build adjacency map
                    const childrenMap = new Map();
                    for (const edge of edges) {
                        if (!childrenMap.has(edge.from)) {
                            childrenMap.set(edge.from, []);
                        }
                        childrenMap.has(edge.to) || childrenMap.set(edge.to, []);
                        childrenMap.get(edge.from).push(edge.to);
                    }

                    // Build tree levels using BFS
                    const levels = [];
                    const nodeMap = new Map(nodes.map(n => [n.id, n]));
                    const visited = new Set();
                    
                    if (nodes.length > 0) {
                        const queue = [[nodes[0].id, 0]];
                        visited.add(nodes[0].id);
                        
                        while (queue.length > 0) {
                            const [nodeId, level] = queue.shift();
                            if (!levels[level]) levels[level] = [];
                            levels[level].push(nodeId);
                            
                            const children = childrenMap.get(nodeId) || [];
                            for (const childId of children) {
                                if (!visited.has(childId)) {
                                    visited.add(childId);
                                    queue.push([childId, level + 1]);
                                }
                            }
                        }
                    }

                    // Render tree
                    const treeEl = document.getElementById('tree');
                    const container = document.createElement('div');
                    container.className = 'tree-container';
                    treeEl.appendChild(container);

                    for (const level of levels) {
                        const levelDiv = document.createElement('div');
                        levelDiv.className = 'tree-level';
                        
                        for (const nodeId of level) {
                            const node = nodeMap.get(nodeId);
                            const nodeEl = document.createElement('div');
                            nodeEl.className = 'tree-node';
                            nodeEl.innerHTML = `<div class="label">Data</div><div class="value">${node.label}</div>`;
                            nodeEl.title = `Address: ${node.id}`;
                            levelDiv.appendChild(nodeEl);
                        }
                        
                        container.appendChild(levelDiv);
                    }
                </script>
            </body>
            </html>
        )";
    }

    template <typename T, typename DataFunc, typename ChildrenFunc>
    inline void writeTreeHTMLFile(
        T* rootNode, 
        const std::string &path,
        DataFunc getData,
        ChildrenFunc getChildren
    ) 
    {
        std::pair<std::string, std::string> nodesEdgesJson = getTreeEdgesJson(
            rootNode, 
            getData,
            getChildren
        );

        const std::string& nodesJson = nodesEdgesJson.first;
        const std::string& edgesJson = nodesEdgesJson.second;

        std::string htmlContent = getTreeHtmlTemplate(nodesJson, edgesJson);

        std::ofstream out(path);
        out << htmlContent;
        out.close();

        std::cout << "The tree html file has been written to: " << path << std::endl;

        std::string launchCommand = "";
        int osType = osdetecter::OsType();

        switch (osType) {
			case 0:
                launchCommand += "start " + path;
				break;
			case 1:
				launchCommand += "google-chrome " + path;
				break;
			case 2:
                launchCommand += "xdg-open " + path;
				break;
			case 3:
				break;
			default:
				break;
		}

        std::system(launchCommand.c_str());
        return;
    }

    template <LinearNode T>
	inline void writeListHTMLFile(T *rootNode, const std::string &path)
	{
		writeListHTMLFile(rootNode, path, [](T *node) { return node->data; },
                                          [](T *node) { return node->next; });
	}

    template <typename T, typename DataFunc, typename ChildrenFunc>
    inline void writeListHTMLFile(T* rootNode, const std::string &path, DataFunc getData, ChildrenFunc getChildren) 
    {
        std::pair<std::string, std::string> nodesEdgesJson = getNodesEdgesJson(
            rootNode,
            getData,
            getChildren
        );

		const std::string& nodesJson = nodesEdgesJson.first;
		const std::string& edgesJson = nodesEdgesJson.second;

		std::string htmlContent = getListHtmlTemplate(nodesJson, edgesJson);

		std::ofstream out(path);
		out << htmlContent;
		out.close();

		std::cout << "The html file has been written to: " << path << std::endl;

		std::string command = "";
		int osType = osdetecter::OsType();

		switch (osType) {
			case 0:
                command += "start " + path;
				break;
			case 1:
				command += "google-chrome " + path;
				break;
			case 2:
                command += "xdg-open " + path;
				break;
			case 3:
				break;
			default:
				break;
		}

		std::system(command.c_str());

		return;
    }

} // namespace vis