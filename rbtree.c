#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义颜色枚举
typedef enum { RED, BLACK } Color;

typedef int KEY_TYPE;

// 定义红黑树节点结构
typedef struct _rbtree_node {
    KEY_TYPE key;
    void *value;
    struct _rbtree_node *left, *right, *parent;
    Color color;
} rbtree_node;

// 定义红黑树结构
typedef struct _rbtree {
    rbtree_node *root;
    rbtree_node *nil;
} rbtree;

// 初始化红黑树
rbtree *rbtree_create() {
    rbtree *tree = (rbtree *)malloc(sizeof(rbtree));
    if (tree == NULL) {
        printf("malloc rbtree failed\n");
        return NULL;
    }

    tree->nil = (rbtree_node *)malloc(sizeof(rbtree_node));
    if (tree->nil == NULL) {
        printf("malloc rbtree_node failed\n");
        free(tree);
        return NULL;
    }
    tree->nil->color = BLACK;
    tree->root = tree->nil;
    return tree;
}

// 创建新的红黑树节点
rbtree_node *rbtree_node_create(rbtree *tree, int key, void *value) {
    rbtree_node *node = (rbtree_node *)malloc(sizeof(rbtree_node));
    node->key = key;
    node->value = value;
    node->color = RED;
    node->left = node->right = node->parent = tree->nil;
    return node;
}

// 左旋
void rbtree_left_rotate(rbtree *tree, rbtree_node *x) {
    rbtree_node *y = x->right;

    x->right = y->left;
    if (y->left != tree->nil) {
        y->left->parent = x;
    }

    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

// 右旋
void rbtree_right_rotate(rbtree *tree, rbtree_node *y) {
    rbtree_node *x = y->left;

    y->left = x->right;
    if (x->right != tree->nil) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y->parent == tree->nil) {
        tree->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}

// 插入修正
void rbtree_insert_fixup(rbtree *tree, rbtree_node *z) {
    while (z->parent->color == RED) { //z->color == RED
        if (z->parent == z->parent->parent->left) {
            rbtree_node *y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;

                z = z->parent->parent; //z->color == RED
            } else { //y->color == BLACK
                if (z == z->parent->right) {
                    z = z->parent;
                    rbtree_left_rotate(tree, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_right_rotate(tree, z->parent->parent);
            }
        } else {
            rbtree_node *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; //z->color == RED
            } else { //y->color == BLACK
                if (z == z->parent->left) {
                    z = z->parent;
                    rbtree_right_rotate(tree, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

// 插入节点
// 红黑树，在插入节点以前，它已经是一棵红黑树
void rbtree_insert(rbtree *tree, rbtree_node *z) {
    rbtree_node *y = tree->nil;
    rbtree_node *x = tree->root;

    while (x != tree->nil) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else if (z->key > x->key) {
            x = x->right;
        } else { //Exist
            return ;
        }
    }

    z->parent = y;
    if (y == tree->nil) {
        tree->root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = tree->nil;
    z->right = tree->nil;
    z->color = RED;

    rbtree_insert_fixup(tree, z);
}

rbtree_node *rbtree_mini(rbtree *tree, rbtree_node *x) {
    while (x->left != tree->nil) {
        x = x->left;
    }
    return x;
}

rbtree_node *rbtree_maxi(rbtree *tree, rbtree_node *x) {
    while (x->right != tree->nil) {
        x = x->right;
    }
    return x;
}

rbtree_node *rbtree_successor(rbtree *tree, rbtree_node *x) {
    rbtree_node *y = x->parent;

    if (x->right != tree->nil) {
        return rbtree_mini(tree, x->right);
    }

    while ((y != tree->nil) && (x == y->right)) {
        x = y;
        y = y->parent;
    }
    return y;
}

// 删除修正
void rbtree_delete_fixup(rbtree *tree, rbtree_node *x) {
    while ((x != tree->root) && (x->color == BLACK)) {
        if (x == x->parent->left) {
            rbtree_node *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;

                rbtree_left_rotate(tree, x->parent);
                w = x->parent->right;
            }

            if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rbtree_right_rotate(tree, w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rbtree_left_rotate(tree, x->parent);

                x = tree->root;
            }
        } else {
            rbtree_node *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rbtree_right_rotate(tree, x->parent);
                w = x->parent->left;
            }

            if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rbtree_left_rotate(tree, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rbtree_right_rotate(tree, x->parent);

                x = tree->root;
            }
        }
    }

    x->color = BLACK;
}

// 删除节点
rbtree_node *rbtree_delete(rbtree *tree, rbtree_node *z) {
    rbtree_node *y = tree->nil;
    rbtree_node *x = tree->nil;

    if ((z->left == tree->nil) || (z->right == tree->nil)) {
        y = z;
    } else {
        y = rbtree_successor(tree, z);
    }

    if (y->left != tree->nil) {
        x = y->left;
    } else if (y->right != tree->nil) {
        x = y->right;
    }

    x->parent = y->parent;
    if (y->parent == tree->nil) {
        tree->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    if (y != z) {
        z->key = y->key;
        z->value = y->value;
    }

    if (y->color == BLACK) {
        rbtree_delete_fixup(tree, x);
    }

    return y;
}

// 中序遍历
void rbtree_inorder_traversal(rbtree *tree, rbtree_node *node) {
    if (node != tree->nil) {
        rbtree_inorder_traversal(tree, node->left);
        printf("key:%d, color:%d\n", node->key, node->color);
        rbtree_inorder_traversal(tree, node->right);
    }
}

rbtree_node *rbtree_search(rbtree *tree, KEY_TYPE key) {
    rbtree_node *node = tree->root;
    while (node != tree->nil) {
        if (key < node->key) {
            node = node->left;
        } else if (key > node->key) {
            node = node->right;
        } else {
            return node;
        }
    }
    return tree->nil;
}

int main() {
    rbtree *tree = rbtree_create();
    if (tree == NULL) return -1;

    int keyArray[] = {24,25,13,35,23,26,67,47,38,98,20,19,17,49,12,21,9,18,14,15};
	int key_count = sizeof(keyArray) / sizeof(keyArray[0]);
    for (int i = 0; i < key_count; ++i) {
        rbtree_node *node = rbtree_node_create(tree, keyArray[i], NULL);
        rbtree_insert(tree, node);
    }

    rbtree_inorder_traversal(tree, tree->root);
    printf("----------------------------------------\n");

    for (int i = 0; i < key_count; ++i) {
        rbtree_node *node = rbtree_search(tree, keyArray[i]);
        rbtree_node *cur = rbtree_delete(tree, node);
        free(cur);

        rbtree_inorder_traversal(tree, tree->root);
        printf("----------------------------------------\n");
    }

    return 0;
}