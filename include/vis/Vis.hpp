#pragma once
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

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
	inline void exploreList(ListNode *rootNode)
	{
		while (rootNode != nullptr)
		{
			std::cout << rootNode->data << " -> ";
			rootNode = rootNode->next;
		}
		std::cout << "NULL";
		std::cout << std::endl;
	}

	inline std::string getNodeAddress(ListNode *node)
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

	inline std::string buildHTMLTemplate() {
		return R"(
			<!DOCTYPE html>
			<html lang="en">
			<head>
				<meta charset="UTF-8">
				<meta name="viewport" content="width=device-width, initial-scale=1.0">
				<title>Linked List Render</title>
				<script src="https://cdn.jsdelivr.net/npm/vis-network@10.0.2/standalone/umd/vis-network.min.js"></script>
			</head>
			<body>
				<p> meow meow </p>
				<script type="text/javascript">
				</script>
			</body>
			</html>
		)";
	}

	inline std::pair<std::string, std::string> getNodesEdgesJson(ListNode* rootNode) {
		std::stringstream nodesJson;
		std::stringstream edgesJson;
		
		nodesJson << "[\n";
		edgesJson << "[\n";
		
		bool isRootNode = true;
		for (auto it = rootNode; it != nullptr; it = it->next)
		{
			if (!isRootNode) {
				nodesJson << ",\n";
				edgesJson << ",\n";
			}

			std::string nodeAddr = getNodeAddress(it);
			nodesJson << "  { \"id\": \"" << nodeAddr << "\", \"label\": \"" << it->data << "\" }";

			if (it->next != nullptr)
			{
				std::string nextAddr = getNodeAddress(it->next);
				edgesJson << "  { \"from\": \"" << nodeAddr << "\", \"to\": \"" << nextAddr << "\" }";
			}

			isRootNode = false;
		}
		
		nodesJson << "\n]";
		edgesJson << "\n]"; 
		
		return std::make_pair(nodesJson.str(), edgesJson.str());
	}

	// NOTE: This will output a .html file containing the renderable content
	// opened in browser ofc

	inline void writeListHTMLFile(ListNode *rootNode, const std::string &path)
	{
		std::pair<std::string, std::string> nodesEdgesJson = getNodesEdgesJson(rootNode);

		const std::string& nodesJson = nodesEdgesJson.first;
		const std::string& edgesJson = nodesEdgesJson.second;

		std::string htmlContent = R"(
			<!DOCTYPE html>
			<html lang="en">
			<head>
				<meta charset="UTF-8">
				<meta name="viewport" content="width=device-width, initial-scale=1.0">
				<title>Linked List Render</title>
				<script src="https://cdn.jsdelivr.net/npm/vis-network@10.0.2/standalone/umd/vis-network.min.js"></script>
			</head>
			<body>
				<div id="network" style="width: 100%; height: 100vh;"></div>
				<script type="text/javascript">
					var nodes = new vis.DataSet()" + nodesJson + R"();
					var edges = new vis.DataSet()" + edgesJson + R"();
					var container = document.getElementById('network');
					var data = { nodes: nodes, edges: edges };
					var options = {
						physics: false,
						interaction: {
							dragNodes: true,
							dragView: true,
							zoomView: true
						}
					};
					var network = new vis.Network(container, data, options);
				</script>
			</body>
			</html>
		)";

		std::ofstream out(path);
		out << htmlContent;
		out.close();

		std::cout << "The html file has been written to: " << path << std::endl;

		std::string command = "";
		int osType = osdetecter::OsType();

		switch (osType) {
			case 0:
				break;
			case 1:
				command += "google-chrome " + path;
				break;
			case 2:
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