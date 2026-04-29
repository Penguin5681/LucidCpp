#pragma once
/*
 * Author: Penguin5681
 * License: MIT
 * Contact: pranav@theapplicable.in
 */
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    #define VIS_CXX20_SUPPORTED 1
    #include <concepts>
#else
    #define VIS_CXX20_SUPPORTED 0
#endif
#include <unordered_set>
#include <queue>

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
    namespace detail {
        template <typename T>
        inline T get_node_ptr(const T& val) { return val; }

        template <typename T, typename W>
        inline T get_node_ptr(const std::pair<T, W>& p) { return p.first; }

        template <typename T>
        inline std::string get_edge_weight(const T& val) { return ""; }

        template <typename T, typename W>
        inline std::string get_edge_weight(const std::pair<T, W>& p) {
            std::stringstream ss;
            ss << p.second;
            return ss.str();
        }

        template <typename T>
        inline std::string get_node_id(T* ptr) {
            std::stringstream ss;
            ss << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
            return ss.str();
        }

        template <typename T>
        inline std::string get_node_id(const T& val) {
            std::stringstream ss;
            ss << val;
            return ss.str();
        }
    }

    template <typename T>
    inline std::string sanitizeForJson(const T& rawData) {
        std::stringstream ss;
        ss << rawData;
        std::string input = ss.str();
        std::string output;
        
        for (char c : input) {
            if (c == '\n') output += "\\n";       
            else if (c == '\"') output += "\\\"";  
            else if (c == '\\') output += "\\\\";  
            else if (c == '\r') output += "\\r";   
            else if (c == '\t') output += "\\t";   
            else output += c;
        }
        return output;
    }

    #if VIS_CXX20_SUPPORTED
    template <typename T>
    concept LinearNode = requires(T node) {
        { node.data };
        { node.next } -> std::convertible_to<T*>;
    };

    template <LinearNode T>
	inline void writeListHTMLFile(T *rootNode, const std::string &path)
	{
		writeListHTMLFile(rootNode, path, [](T *node) { return node->data; },
                                          [](T *node) { return node->next; });
	}
    #else
    // C++11/14/17 Universe (SFINAE Fallback)
    template <typename T>
    inline auto writeListHTMLFile(T *rootNode, const std::string &path) 
        -> decltype(rootNode->data, rootNode->next, void()) 
    {
        writeListHTMLFile(rootNode, path, 
            [](T* n) { return n->data; }, 
            [](T* n) { return n->next; }
        );
    }
    #endif

    template <typename T>
	inline void exploreStructure(T *rootNode)
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

    template <typename T>
	inline void exploreListAddressing(T *rootNode)
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
    template <typename T>
	inline void writeListDotFile(T *rootNode,
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
    inline std::tuple<std::string, std::string, bool> getTreeEdgesJson(T* rootNode, DataFunc getData, ChildrenFunc getChildren) {
        std::unordered_set<const void*> visitedNodes;
        std::queue<T*> q;
        std::stringstream nodesJson;
        std::stringstream edgesJson;

        struct EdgeInfo {
            std::string from;
            std::string to;
            std::string weight;
        };
        std::vector<EdgeInfo> all_edges;

        nodesJson << "[\n";
        bool isFirstNode = true;

        if (rootNode != nullptr) {
            q.push(rootNode);
            visitedNodes.insert(rootNode);
        }

        while (!q.empty()) {
            T* current = q.front();
            q.pop();

            if (!isFirstNode) nodesJson << ",\n";
            nodesJson << "  { \"id\": \"" << detail::get_node_id(current) << "\", \"label\": \"" << sanitizeForJson(getData(current)) << "\" }";
            isFirstNode = false;

            auto children = getChildren(current);
            for (const auto& child_item : children) {
                T* child = detail::get_node_ptr(child_item);
                if (child == nullptr) {
                    continue;
                }
                
                std::string weight = detail::get_edge_weight(child_item);
                all_edges.push_back({detail::get_node_id(current), detail::get_node_id(child), weight});

                if (visitedNodes.find(child) == visitedNodes.end()) {
                    visitedNodes.insert(child);
                    q.push(child);
                }
            }
        }
        nodesJson << "\n]";

        bool is_undirected = true;
        if (all_edges.empty()) {
            is_undirected = false;
        } else {
            std::unordered_set<std::string> edge_set;
            for (const auto& e : all_edges) {
                edge_set.insert(e.from + "->" + e.to + ":" + e.weight);
            }
            for (const auto& e : all_edges) {
                if (edge_set.find(e.to + "->" + e.from + ":" + e.weight) == edge_set.end()) {
                    is_undirected = false;
                    break;
                }
            }
        }

        std::vector<EdgeInfo> final_edges;
        if (is_undirected) {
            std::unordered_set<std::string> seen;
            for (const auto& e : all_edges) {
                std::string key1 = e.from + "->" + e.to;
                std::string key2 = e.to + "->" + e.from;
                if (seen.find(key1) == seen.end() && seen.find(key2) == seen.end()) {
                    seen.insert(key1);
                    final_edges.push_back(e);
                }
            }
        } else {
            final_edges = all_edges;
        }

        edgesJson << "[\n";
        bool isFirstEdge = true;
        for (const auto& e : final_edges) {
            if (!isFirstEdge) edgesJson << ",\n";
            edgesJson << "  { \"from\": \"" << e.from << "\", \"to\": \"" << e.to << "\"";
            if (!e.weight.empty()) {
                edgesJson << ", \"label\": \"" << sanitizeForJson(e.weight) << "\"";
            }
            edgesJson << " }";
            isFirstEdge = false;
        }
        edgesJson << "\n]";
        
        return std::make_tuple(nodesJson.str(), edgesJson.str(), is_undirected);
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
			nodesJson << "  { \"id\": \"" << getNodeAddress(it) << "\", \"label\": \"" << sanitizeForJson(getData(it)) << "\" }";
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
                        margin: 0;Back-end development in at least one core stack (Python / Node.js / .NET/C# / PHP/WordPress / C++), RESTful API design & server architecture (OOP, scalable systems), Database engineering (SQL + NoSQL, data modeling, performance, integrity/security), Testing & code quality (test strategy, coverage, refactoring/debugging), Remote collaboration & engineering workflow (Git, distributed communication; CI/CD/containers a plus)

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
                    .container {
                        overflow-x: auto;
                        padding: 40px 24px 32px;
                    }
                </style>
            </head>
            <body>
                <div class="header">
                    <h1>Linked List</h1>
                    <p>SVG Based Memory Renderer</p>
                </div>
                <div class="container">
                    <svg id="canvas" height="350px" style="border: 1px #deb5aa solid;" xmlns="http://www.w3.org/2000/svg">
                        </svg>
                </div>
                <script type="text/javascript">
                    const nodes = )" + nodesJson + R"(;
                    const edges = )" + edgesJson + R"(;

                    const nextMap = new Map();
                    for (const edge of edges) {
                        nextMap.set(edge.from, edge.to);
                    }

                    const canvas = document.getElementById('canvas');
                    const xw = 250;
                    
                    // FIX 1: Add 'px' to the width assignment
                    canvas.style.width = (nodes.length * xw + 50) + "px"; 

                    for (let i = 0; i < nodes.length; i++) {
                        const node = nodes[i];
                        const nextAddr = nextMap.get(node.id) || 'NULL';

                        // Truncate nextAddr to keep UI clean if it's too long
                        const displayAddr = nextAddr.length > 14 ? nextAddr.substring(0, 12) + '...' : nextAddr;

                        canvas.innerHTML += '<rect class="next" width="150" height="100" x="'+(xw*i + 70)+'" y="20" rx="20" ry="20" fill="#deb5aa" stroke="white" stroke-width="1px"/>';
                        canvas.innerHTML += '<text x="'+(xw*i + 105)+'" y="50" font-size="12" fill="white">NEXT</text>';
                        canvas.innerHTML += '<rect class="add" width="105" height="50" rx="10" ry="10" x="'+(xw*i + 100)+'" y="60"  fill="white" stroke="black" stroke-dasharray="2,2" stroke-width="1px"/>';
                        canvas.innerHTML += '<text x="'+(xw*i + 106)+'" y="88" font-size="11" font-family="monospace" fill="black">'+displayAddr+'</text>';
                        
                        canvas.innerHTML += '<rect class="data" id="node'+i+'" width="70" height="100" x="'+(xw*i + 20)+'" y="20" rx="20" ry="20" fill="#deb5aa" stroke="white" stroke-width="1px"/>';
                        canvas.innerHTML += '<text x="'+(xw*i + 40)+'" y="50" font-size="12" fill="white">DATA</text>';
                        canvas.innerHTML += '<text x="'+(xw*i + 40)+'" y="85" font-size="16" fill="white">'+(node.label)+'</text>';

                        if (i < nodes.length - 1) {
                            canvas.innerHTML += '<path d="M'+(250*(i+1)+1.883)+' 70 l-7.527 6.235.644.765 9-7.521-9-7.479-.645.764 7.529 6.236h-21.884v1h21.883z" fill="var(--accent)\" />';
                        }
                    }

                    const lastNode = nodes[nodes.length - 1];
                    const finalTargetAddr = nextMap.get(lastNode.id);

                    if (finalTargetAddr) {
                        // FIX 2: Find the array index of the target node instead of using its string label for math
                        const targetIndex = nodes.findIndex(n => n.id === finalTargetAddr);
                        
                        if (targetIndex !== -1) {
                            // Calculate the math based on the index (0-based) instead of the label (1-based)
                            // 250 * (targetIndex + 1) - 195  ==> 250 * targetIndex + 55
                            const startX = 250 * targetIndex + 55;
                            const widthJump = 250 * (nodes.length - 1 - targetIndex) + 100;

                            canvas.innerHTML += '<path d="M'+startX+' 120 v50 h'+widthJump+' v-50 v50 h-'+widthJump+' c20 30 Z" stroke="#e07a5f" fill="none" stroke-opacity="0.9" stroke-width="2"  stroke-linecap="round" stroke-linejoin="round"></path>';
                            canvas.innerHTML += '<path d="M'+startX+' 120 l-10 10 l20 0 Z" fill="#e07a5f" stroke="#e07a5f" stroke-opacity="0.9" stroke-width="1" stroke-linecap="round" stroke-linejoin="round"></path>';
                        }
                    } 
                </script>
            </body>
            </html>
        )";
    }
	// NOTE: This will output a .html file containing the renderable content
	// opened in browser ofc

    inline std::string getTreeHtmlTemplate(const std::string& nodesJson, const std::string& edgesJson, bool isUndirected) {
        std::string arrowsConfig = isUndirected ? "arrows: { to: { enabled: false } }," : "arrows: { to: { enabled: true, scaleFactor: 0.7 } },";
        return R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Tree Data Structure Render</title>
                <script type="text/javascript" src="https://unpkg.com/vis-network/standalone/umd/vis-network.min.js"></script>
                <style>
                    :root {
                        --bg: #f5f2ea;
                        --ink: #1b1b1b;
                        --accent: #e07a5f;
                        --panel: #fff7e8;
                        --border: #2f2f2f;
                    }
                    body, html {
                        margin: 0;
                        padding: 0;
                        width: 100%;
                        height: 100%;
                        font-family: "Space Grotesk", "Trebuchet MS", sans-serif;
                        background: radial-gradient(1200px 800px at 20% 10%, #fff2d8, var(--bg));
                        color: var(--ink);
                        overflow: hidden; 
                    }
                    .header {
                        position: absolute;
                        top: 0;
                        left: 0;
                        padding: 20px 24px;
                        z-index: 10;
                        pointer-events: none; 
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
                    #tree-network {
                        width: 100%;
                        height: 100%;
                        position: absolute;
                        top: 0;
                        left: 0;
                    }
                </style>
            </head>
            <body>
                <div class="header">
                    <h1>Tree / Graph View</h1>
                    <p>Hierarchical Layout. Drag to pan, scroll to zoom.</p>
                </div>
                
                <div id="tree-network"></div>

                <script type="text/javascript">
                    const nodesArray = )" + nodesJson + R"(;
                    const edgesArray = )" + edgesJson + R"(;

                    const nodes = new vis.DataSet(nodesArray);
                    const edges = new vis.DataSet(edgesArray);

                    const container = document.getElementById('tree-network');
                    const data = { nodes: nodes, edges: edges };

                    const options = {
                        layout: {
                            hierarchical: {
                                direction: 'UD',       
                                sortMethod: 'directed', 
                                levelSeparation: 120,   
                                nodeSpacing: 180,       // INCREASED: Gives wider boxes more breathing room
                                treeSpacing: 200        
                            }
                        },
                        physics: { enabled: false },
                        interaction: {
                            dragNodes: true, 
                            zoomView: true,
                            dragView: true
                        },
                        nodes: {
                            shape: 'box', // CHANGED: 'box' expands dynamically to fit text
                            margin: { top: 12, bottom: 12, left: 16, right: 16 }, // Adds inner padding
                            widthConstraint: { 
                                maximum: 200 // CRITICAL: Forces text longer than 200px to wrap to a new line
                            },
                            font: { 
                                face: 'Space Grotesk', 
                                color: '#1b1b1b',
                                size: 16,
                                bold: true,
                                multi: 'html' // Allows you to use <b> or <i> tags in your C++ data if you want!
                            },
                            borderWidth: 2,
                            color: {
                                background: '#fff7e8',
                                border: '#2f2f2f',
                                highlight: { background: '#e07a5f', border: '#1b1b1b' }
                            },
                            shadow: { enabled: true, color: 'rgba(0,0,0,0.1)', size: 5, x: 3, y: 3 }
                        },
                        edges: {
                            )" + arrowsConfig + R"(
                            color: { color: '#2f2f2f', highlight: '#e07a5f' },
                            font: { align: 'top', background: '#fff7e8', strokeWidth: 0 },
                            width: 2,
                            smooth: {
                                type: 'cubicBezier',      
                                forceDirection: 'vertical', 
                                roundness: 0.5
                            }
                        }
                    };

                    const network = new vis.Network(container, data, options);
                </script>
            </body>
            </html>
        )";
    }

    template <typename GraphType, typename GetNodesFunc, typename DataFunc, typename ChildrenFunc>
    inline std::tuple<std::string, std::string, bool> getGraphEdgesJson(
        GraphType& graph, 
        GetNodesFunc getNodes, 
        DataFunc getData, 
        ChildrenFunc getChildren) 
    {
        std::stringstream nodesJson;
        std::stringstream edgesJson;

        struct EdgeInfo {
            std::string from;
            std::string to;
            std::string weight;
        };
        std::vector<EdgeInfo> all_edges;

        nodesJson << "[\n";
        bool isFirstNode = true;

        auto nodes = getNodes(graph);
        for (const auto& node_item : nodes) {
            auto current = detail::get_node_ptr(node_item);

            if (!isFirstNode) nodesJson << ",\n";
            nodesJson << "  { \"id\": \"" << detail::get_node_id(current) << "\", \"label\": \"" << sanitizeForJson(getData(current)) << "\" }";
            isFirstNode = false;

            auto children = getChildren(current);
            for (const auto& child_item : children) {
                auto child = detail::get_node_ptr(child_item);
                
                std::string weight = detail::get_edge_weight(child_item);
                all_edges.push_back({detail::get_node_id(current), detail::get_node_id(child), weight});
            }
        }
        nodesJson << "\n]";

        bool is_undirected = true;
        if (all_edges.empty()) {
            is_undirected = false;
        } else {
            std::unordered_set<std::string> edge_set;
            for (const auto& e : all_edges) {
                edge_set.insert(e.from + "->" + e.to + ":" + e.weight);
            }
            for (const auto& e : all_edges) {
                if (edge_set.find(e.to + "->" + e.from + ":" + e.weight) == edge_set.end()) {
                    is_undirected = false;
                    break;
                }
            }
        }

        std::vector<EdgeInfo> final_edges;
        if (is_undirected) {
            std::unordered_set<std::string> seen;
            for (const auto& e : all_edges) {
                std::string key1 = e.from + "->" + e.to;
                std::string key2 = e.to + "->" + e.from;
                if (seen.find(key1) == seen.end() && seen.find(key2) == seen.end()) {
                    seen.insert(key1);
                    final_edges.push_back(e);
                }
            }
        } else {
            final_edges = all_edges;
        }

        edgesJson << "[\n";
        bool isFirstEdge = true;
        for (const auto& e : final_edges) {
            if (!isFirstEdge) edgesJson << ",\n";
            edgesJson << "  { \"from\": \"" << e.from << "\", \"to\": \"" << e.to << "\"";
            if (!e.weight.empty()) {
                edgesJson << ", \"label\": \"" << sanitizeForJson(e.weight) << "\"";
            }
            edgesJson << " }";
            isFirstEdge = false;
        }
        edgesJson << "\n]";
        
        return std::make_tuple(nodesJson.str(), edgesJson.str(), is_undirected);
    }

    template <typename GraphType, typename GetNodesFunc, typename DataFunc, typename ChildrenFunc>
    inline void writeGraphHTMLFile(
        GraphType& graph, 
        const std::string &path,
        GetNodesFunc getNodes,
        DataFunc getData,
        ChildrenFunc getChildren
    ) 
    {
        auto nodesEdgesJson = getGraphEdgesJson(
            graph, 
            getNodes,
            getData,
            getChildren
        );

        const std::string& nodesJson = std::get<0>(nodesEdgesJson);
        const std::string& edgesJson = std::get<1>(nodesEdgesJson);
        bool isUndirected = std::get<2>(nodesEdgesJson);

        std::string htmlContent = getTreeHtmlTemplate(nodesJson, edgesJson, isUndirected);

        std::ofstream out(path);
        out << htmlContent;
        out.close();

        std::cout << "The graph html file has been written to: " << path << std::endl;

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

    template <typename T, typename DataFunc, typename ChildrenFunc>
    inline void writeTreeHTMLFile(
        T* rootNode, 
        const std::string &path,
        DataFunc getData,
        ChildrenFunc getChildren
    ) 
    {
        auto nodesEdgesJson = getTreeEdgesJson(
            rootNode, 
            getData,
            getChildren
        );

        const std::string& nodesJson = std::get<0>(nodesEdgesJson);
        const std::string& edgesJson = std::get<1>(nodesEdgesJson);
        bool isUndirected = std::get<2>(nodesEdgesJson);

        std::string htmlContent = getTreeHtmlTemplate(nodesJson, edgesJson, isUndirected);

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