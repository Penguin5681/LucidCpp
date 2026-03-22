#include <iostream>
#include <string>
#include <vector>
#include "vis/Vis.hpp" 

struct TreeNode {
    std::string val;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

struct FileNode {
    std::string name;
    std::vector<FileNode*> children;
};

TreeNode* buildMassiveTree(int currentDepth, int maxDepth, std::string path) {
    if (currentDepth > maxDepth) return nullptr;
    TreeNode* node = new TreeNode{"Node\n" + path};
    node->left = buildMassiveTree(currentDepth + 1, maxDepth, path + "L");
    node->right = buildMassiveTree(currentDepth + 1, maxDepth, path + "R");
    return node;
}

int main() {
    std::cout << "[Test 1] Generating N-ary Tree (File System Simulation)...\n";
    FileNode* rootDir = new FileNode{"📂 root"};
    FileNode* usrDir = new FileNode{"📂 usr"};
    FileNode* varDir = new FileNode{"📂 var"};
    FileNode* homeDir = new FileNode{"📂 home"};
    
    rootDir->children = {usrDir, varDir, homeDir};
    usrDir->children = {new FileNode{"📄 bin"}, new FileNode{"📄 lib"}};
    homeDir->children = {new FileNode{"📄 user1.txt"}, new FileNode{"📄 secret.key"}};
    
    vis::writeTreeHTMLFile(rootDir, "test1_n_ary_tree.html", 
        [](FileNode* n) { return n->name; },
        [](FileNode* n) { return n->children; }
    );


    std::cout << "[Test 2] Generating HTML & Responsive Text Stress Test...\n";
    TreeNode* htmlRoot = new TreeNode{"<b>Root Node</b><br><i>(HTML Enabled)</i>"};
    htmlRoot->left = new TreeNode{"This is a ridiculously long string meant to test the 200px width constraint of our Vis.js boxes. It should automatically wrap to a new line!"};
    htmlRoot->right = new TreeNode{"Short"};
    htmlRoot->right->left = new TreeNode{"🔥 Emojis Work too!"};
    htmlRoot->right->right = new TreeNode{"<span style='color:red'>Inline CSS!</span>"};

    vis::writeTreeHTMLFile(htmlRoot, "test2_html_stress.html", 
        [](TreeNode* n) { return n->val; },
        [](TreeNode* n) { return std::vector<TreeNode*>{n->left, n->right}; }
    );


    std::cout << "[Test 3] Generating DAG & Cycle Break Test...\n";
    TreeNode* a = new TreeNode{"A (Start)"};
    TreeNode* b = new TreeNode{"B"};
    TreeNode* c = new TreeNode{"C"};
    TreeNode* d = new TreeNode{"D (Diamond Bottom)"};

    a->left = b;
    a->right = c;
    b->right = d; 
    c->left = d;  
    d->left = a;  

    vis::writeTreeHTMLFile(a, "test3_cycle_dag.html", 
        [](TreeNode* n) { return n->val; },
        [](TreeNode* n) { return std::vector<TreeNode*>{n->left, n->right}; }
    );

    std::cout << "[Test 4] Generating Massive Scale Tree (Depth 6 - 63 Nodes)...\n";
    TreeNode* massiveRoot = buildMassiveTree(1, 5, "1");

    vis::writeTreeHTMLFile(massiveRoot, "test4_massive_tree.html", 
        [](TreeNode* n) { return n->val; },
        [](TreeNode* n) { return std::vector<TreeNode*>{n->left, n->right}; }
    );

    std::cout << "All stress tests completed! Check your browser windows.\n";
    return 0;
}