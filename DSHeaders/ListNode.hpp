#include <iostream>

class ListNode
{
  public:
	int data;
	ListNode *next;

	ListNode(int data)
	{
		this->data = data;
		next = nullptr;
	}
};

void insertAtEnd(ListNode *&head, int newData)
{
	ListNode *newNode = new ListNode(newData);
	if (head == nullptr)
	{
		head = newNode;
		return;
	}
	ListNode *temp = head;
	while (temp->next != nullptr)
	{
		temp = temp->next;
	}
	temp->next = newNode;
}

void insertAtBeginning(ListNode *&head, int newData)
{
	ListNode *newNode = new ListNode(newData);
	newNode->next = head;
	head = newNode;
}

void insertAtPosition(ListNode *&head, int newData, int position)
{
	if (position == 0)
	{
		insertAtBeginning(head, newData);
		return;
	}
	ListNode *temp = head;
	for (int i = 0; i < position - 1 && temp != nullptr; i++)
	{
		temp = temp->next;
	}
	if (temp == nullptr)
		return;
	ListNode *newNode = new ListNode(newData);
	newNode->next = temp->next;
	temp->next = newNode;
}

void deleteAtBeginning(ListNode *&head)
{
	if (head == nullptr)
		return;
	ListNode *temp = head;
	head = head->next;
	delete temp;
}

void deleteAtEnd(ListNode *&head)
{
	if (head == nullptr)
		return;
	if (head->next == nullptr)
	{
		delete head;
		head = nullptr;
		return;
	}
	ListNode *temp = head;
	while (temp->next->next != nullptr)
	{
		temp = temp->next;
	}
	delete temp->next;
	temp->next = nullptr;
}

void deleteAtPosition(ListNode *&head, int position)
{
	if (head == nullptr)
		return;
	if (position == 0)
	{
		deleteAtBeginning(head);
		return;
	}
	ListNode *temp = head;
	for (int i = 0; i < position - 1 && temp->next != nullptr; i++)
	{
		temp = temp->next;
	}
	if (temp->next == nullptr)
		return;
	ListNode *nodeToDelete = temp->next;
	temp->next = temp->next->next;
	delete nodeToDelete;
}

void display(ListNode *head)
{
	ListNode *temp = head;
	while (temp != nullptr)
	{
		std::cout << temp->data << " -> ";
		temp = temp->next;
	}
	std::cout << "nullptr\n";
}

int search(ListNode *head, int key)
{
	ListNode *temp = head;
	int position = 0;
	while (temp != nullptr)
	{
		if (temp->data == key)
			return position;
		temp = temp->next;
		position++;
	}
	return -1;
}

void reverseList(ListNode *&head)
{
	ListNode *prev = nullptr;
	ListNode *current = head;
	while (current != nullptr)
	{
		ListNode *nextTemp = current->next;
		current->next = prev;
		prev = current;
		current = nextTemp;
	}
	head = prev;
}

void deleteList(ListNode *&head)
{
	while (head != nullptr)
	{
		deleteAtBeginning(head);
	}
}

void createCycle(ListNode* &head, int cyclePosition)
{
	if (head == nullptr || head->next == nullptr)
		return;
	ListNode *temp = head;
	int listLength = 0;
	while (temp->next != nullptr)
	{
		temp = temp->next;
		listLength++;
	}
	if (cyclePosition < 0 || cyclePosition > listLength)
		return;
	ListNode *cycleNode = head;
	for (int i = 0; i < cyclePosition; i++)
	{
		cycleNode = cycleNode->next;
	}
	temp->next = cycleNode;
}
