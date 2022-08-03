#ifndef tree_H
#define tree_H

// #include <string.h>

typedef struct TreeNode_t {
    char *key;
    void *payload;
    struct TreeNode_t *left;
    struct TreeNode_t *right;
} TreeNode_t;

typedef struct Tree_t {
    TreeNode_t *root;
} Tree_t;

Tree_t *Tree_new();
void *Tree_get(Tree_t *tree, char *key);
void Tree_insert(Tree_t *tree, char *key, void *payload);
void Tree_traverse(Tree_t *tree, void (*treeTraverseFunc)(void *in, void **data), void **data);
void Node_traverse(TreeNode_t *node, void (*treeTraverseFunc)(void *in, void **data), void **data);



#endif