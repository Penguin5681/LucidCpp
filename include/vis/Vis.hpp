#pragma once
#include "../DSHeaders/ListNode.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

	inline void writeListDotFile(ListNode *rootNode, const std::string &path)
	{
		std::ofstream out(path);
		out << "digraph G {\n";
		for (auto it = rootNode; it != nullptr; it = it->next) {
			out << "\"" << getNodeAddress(it) << "\"";
			out << " [label=\"" << it->data << "\"];\n";
			out << "\"" << getNodeAddress(it) << "\"";
			out << " -> \"" << getNodeAddress(it->next) << "\";\n";
			out << std::endl;
		}
		out << "}\n";
		out.close();
	}
} // namespace vis