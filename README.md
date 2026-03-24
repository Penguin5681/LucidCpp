## LucidCpp

Header-only C++ data structure visualizer. Generate an HTML view for lists and trees without changing your structs.

## Requirements

- C++11 or newer
- A web browser (opens automatically)

## Getting started

1. Copy the folder [include/vis](include/vis) into your project.
2. Include the header:
   ```cpp
   #include "vis/Vis.hpp"
   ```
3. Build and run your program.

## Tutorial: visualize a linked list

Define a node type, then call `vis::writeListHTMLFile`.

```cpp
#include "vis/Vis.hpp"

struct Node {
   int data;
   Node* next;
};

int main() {
   Node c{3, nullptr};
   Node b{2, &c};
   Node a{1, &b};

   vis::writeListHTMLFile(&a, "list.html");
   return 0;
}
```

Output: an HTML file is created and opened in your browser.

## Tutorial: visualize a tree

For trees (or graphs), pass lambdas that describe how to read node data and children.

```cpp
#include "vis/Vis.hpp"
#include <vector>

struct TreeNode {
   std::string value;
   TreeNode* left;
   TreeNode* right;
};

int main() {
   TreeNode c{"C", nullptr, nullptr};
   TreeNode b{"B", nullptr, nullptr};
   TreeNode a{"A", &b, &c};

   auto getData = [](TreeNode* n) { return n->value; };
   auto getChildren = [](TreeNode* n) {
      std::vector<TreeNode*> kids;
      if (n->left) kids.push_back(n->left);
      if (n->right) kids.push_back(n->right);
      return kids;
   };

   vis::writeTreeHTMLFile(&a, "tree.html", getData, getChildren);
   return 0;
}
```

## Examples

- Linked list example: [examples/basic_list.cpp](examples/basic_list.cpp)
- Tree example: [examples/basic_tree.cpp](examples/basic_tree.cpp)