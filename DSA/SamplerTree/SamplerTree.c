#include <stdio.h>
#include <stdbool.h>

#define MAX_SIZE 100

// Check whether the array represents a valid binary tree
bool isValidTree(int tree[], int n) {
    // The root must exist
    if (n == 0 || tree[0] == -1) {
        return false;
    }

    for (int i = 1; i < n; i++) {
        // Ignore empty positions
        if (tree[i] == -1) {
            continue;
        }

        // Find parent index
        int parent = (i - 1) / 2;

        // A node cannot exist if its parent is empty
        if (tree[parent] == -1) {
            return false;
        }
    }

    return true;
}

// Print the tree array
void printTree(int tree[], int n) {
    printf("\nArray representation:\n");

    for (int i = 0; i < n; i++) {
        printf("Index %d: %d\n", i, tree[i]);
    }
}

// Inorder traversal
void inorder(int tree[], int n, int index) {
    if (index >= n || tree[index] == -1) {
        return;
    }

    inorder(tree, n, 2 * index + 1);
    printf("%d ", tree[index]);
    inorder(tree, n, 2 * index + 2);
}

// Preorder traversal
void preorder(int tree[], int n, int index) {
    if (index >= n || tree[index] == -1) {
        return;
    }

    printf("%d ", tree[index]);
    preorder(tree, n, 2 * index + 1);
    preorder(tree, n, 2 * index + 2);
}

// Postorder traversal
void postorder(int tree[], int n, int index) {
    if (index >= n || tree[index] == -1) {
        return;
    }

    postorder(tree, n, 2 * index + 1);
    postorder(tree, n, 2 * index + 2);
    printf("%d ", tree[index]);
}

int main() {
    int tree[MAX_SIZE];
    int n = 0;

    printf("Enter tree elements (-1 for an empty node, 0 to finish):\n");
    while (n < MAX_SIZE) {
        int value;
        if (scanf("%d", &value) != 1) {
            printf("Invalid input.\n");
            return 1;
        }
        if (value == 0) {
            break;
        }
        tree[n++] = value;
    }

    // Validate the tree
    if (!isValidTree(tree, n)) {
        printf("\nInvalid binary tree!\n");
        printf("A node cannot have an empty parent.\n");
        return 0;
    }

    printf("\nValid binary tree!\n");

    printTree(tree, n);

    printf("\nPreorder: ");
    preorder(tree, n, 0);

    printf("\nInorder: ");
    inorder(tree, n, 0);

    printf("\nPostorder: ");
    postorder(tree, n, 0);

    printf("\n");

    return 0;
}
