#ifndef _AVL_H_
#define _AVL_H_

typedef struct AVLNode {

    void* id;
    char balance;
    struct AVLNode* left, *right;

} AVLNode;

typedef int (*Comparator)(const void*, const void*);

typedef struct AVLTree {

    AVLNode* root;
    Comparator comparator;
    void (*destroy)(void *data);

} AVLTree;

extern AVLTree *avl_init (Comparator comparator, void (*destroy)(void *data));
extern void avl_clearTree (AVLNode **node, void (*destroy)(void *data));
extern bool avl_isEmpty (AVLTree *tree);
extern void *avl_getNodeData (AVLTree *tree, void *id);
extern void avl_insertNode (AVLTree *tree, void *data);
extern void *avl_removeNode (AVLTree *tree, void *data);
extern bool avl_nodeInTree (AVLTree *tree, void *id);

#endif
