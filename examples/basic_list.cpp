#include "vis/Vis.hpp"
#include <iostream>

int main()
{
	ListNode *rootNode = new ListNode(1);

	for (int i = 2; i != 11; i++)
	{
		insertAtEnd(rootNode, i);
	}

	vis::explore("meow meow. The libray is working");
	// vis::exploreList(rootNode);
	// vis::exploreListAddressing(rootNode);

	// vis::writeListDotFile(rootNode, "list.dot");
	vis::writeListHTMLFile(rootNode, "index.html");

	return 0;
}