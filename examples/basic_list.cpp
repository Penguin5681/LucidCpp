#include "vis/Vis.hpp"
#include <iostream>
#include "../DSHeaders/ListNode.hpp"

// this is something a nigga would create
struct WeirdStudent {
    std::string name;
    WeirdStudent* nextStudent;
};

int main()
{
	ListNode *rootNode = new ListNode(1);

	for (int i = 2; i != 111; i++)
	{
		insertAtEnd(rootNode, i);
	}

	createCycle(rootNode, 41);

	vis::explore("meow meow. The libray is working");
	// vis::exploreStructure(rootNode);
	// vis::exploreListAddressing(rootNode);

	// vis::writeListDotFile(rootNode, "list.dot");
	vis::writeListHTMLFile(rootNode, "index.html");

	return 0;
}
