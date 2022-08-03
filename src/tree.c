#include <assert.h>
#include <stdio.h>
#include "tree.h"
#include "core/structures/control/mem.h"

Tree_t *Tree_new() {
    Tree_t *tree;
    NEW(tree);
    tree->root = 0;
    return tree;
};

void *Tree_get(Tree_t *tree, char *key) {
    TreeNode_t *n = tree->root;
    while (1) {
        if (!n) {
            return 0;
        }
        int result = strcmp(key, n->key);
        if (result == 0) return n->payload;
        if (result > 0) {
            n = n->right;
        } else {
            n = n->left;
        }
    }
}
void Tree_insert(Tree_t *tree, char *key, void *payload) {
    TreeNode_t **n = &tree->root;
    while (1) {
        if (!*n) {
            NEW(*n);
            (*n)->left = 0;
            (*n)->right = 0;
            (*n)->key = key;
            (*n)->payload = payload;
            break;
        }
        int result = strcmp(key, (*n)->key);
        assert(result != 0);
        if (result > 0) {
            n = &(*n)->left;
        } else {
            n = &(*n)->right;

        }
    }
}

void Node_traverse(TreeNode_t *node, void (*treeTraverseFunc)(void *in, void **data), void **data) {
    if (node) {
        Node_traverse(node->left, treeTraverseFunc, data);
        treeTraverseFunc(node->payload, data);
        Node_traverse(node->right, treeTraverseFunc, data);
    }
}

void Tree_traverse(Tree_t *tree, void (*treeTraverseFunc)(void *in, void **data), void **data) {
    Node_traverse(tree->root, treeTraverseFunc, data);
}
