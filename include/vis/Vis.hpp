#pragma once
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

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

	// NOTE: This will output a .html file containing the renderable content
	// opened in browser ofc

	inline void writeListHTMLFile(ListNode *rootNode, const std::string &path)
	{
		std::ofstream out(path);

		std::cout << "The html file has written to: " << path << std::endl;
		return;
	}
} // namespace vis