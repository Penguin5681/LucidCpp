#include "iostream"
#include "vis/Vis.hpp"

struct TreeNode {
    std::string val;
    TreeNode* left;
    TreeNode* right;
};

int main() {
    TreeNode* root = new TreeNode{"Root", nullptr, nullptr};
    root->left = new TreeNode{"Left Child", nullptr, nullptr};
    root->right = new TreeNode{"Right Child", nullptr, nullptr};
    root->left->left = new TreeNode{"1", nullptr, nullptr};
    root->left->right = new TreeNode{"2", nullptr, nullptr};
    root->right->left = new TreeNode{"3", nullptr, nullptr};
    root->right->right = new TreeNode{"4", nullptr, nullptr};
    root->left->left->left = new TreeNode{"5", nullptr, nullptr};
    root->left->left->right = new TreeNode{"6", nullptr, nullptr};
    root->left->right->left = new TreeNode{"7", nullptr, nullptr};
    root->left->right->right = new TreeNode{"8", nullptr, nullptr};
    root->right->left->left = new TreeNode{"9", nullptr, nullptr};
    root->right->left->right = new TreeNode{"10", nullptr, nullptr};
    root->right->right->left = new TreeNode{"11", nullptr, nullptr};
    root->right->right->right = new TreeNode{"12", nullptr, nullptr};

    vis::writeTreeHTMLFile(root, "tree.html", 
        [](TreeNode* n) { return n->val; },
        [](TreeNode* n) { return std::vector<TreeNode*>{n->left, n->right}; }
    );
}