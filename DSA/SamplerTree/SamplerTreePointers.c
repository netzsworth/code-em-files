#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1000
#define CONTENT_SIZE 120

typedef struct Node {
    int id;
    char content[CONTENT_SIZE];
    struct Node *left;
    struct Node *right;
    int isLeaf;
} Node;

Node *nodeRegistry[MAX_NODES];
Node *root = NULL;
int nodeCount = 0;

void clearInput(void) {
    int character;
    while ((character = getchar()) != '\n' && character != EOF) {}
}

int readInt(int *value) {
    char line[CONTENT_SIZE], extra;
    if (fgets(line, sizeof(line), stdin) == NULL) return 0;
    if (strchr(line, '\n') == NULL && !feof(stdin)) {
        clearInput();
        return 0;
    }
    return sscanf(line, "%d %c", value, &extra) == 1;
}

Node *createNode(int id, const char *content, int isLeaf) {
    Node *node = calloc(1, sizeof(*node));
    if (node == NULL) return NULL;
    node->id = id;
    snprintf(node->content, sizeof(node->content), "%s", content);
    node->isLeaf = isLeaf;
    return node;
}

void freeNodes(void) {
    for (int i = 0; i < nodeCount; i++) free(nodeRegistry[i]);
    memset(nodeRegistry, 0, sizeof(nodeRegistry));
    nodeCount = 0;
    root = NULL;
}

Node *findById(int id) {
    for (int i = 0; i < nodeCount; i++) {
        if (nodeRegistry[i]->id == id) return nodeRegistry[i];
    }
    return NULL;
}

int nodeIndex(Node *target) {
    for (int i = 0; i < nodeCount; i++) {
        if (nodeRegistry[i] == target) return i;
    }
    return -1;
}

void loadSampleTree(void) {
    static const char *contents[] = {
        "DTB < 2.0?", "No >= 12 tin chi?", "DTB >= 3.2?",
        "Canh bao hoc vu muc 2", "Canh bao hoc vu muc 1",
        "De xuat khen thuong", "Theo doi binh thuong"
    };

    freeNodes();
    for (int i = 0; i < 7; i++) {
        nodeRegistry[i] = createNode(i + 1, contents[i], i >= 3);
        if (nodeRegistry[i] == NULL) {
            nodeCount = i;
            freeNodes();
            printf("Khong du bo nho de tao cay mau.\n");
            return;
        }
        nodeCount++;
    }

    root = nodeRegistry[0];
    nodeRegistry[0]->left = nodeRegistry[1];
    nodeRegistry[0]->right = nodeRegistry[2];
    nodeRegistry[1]->left = nodeRegistry[3];
    nodeRegistry[1]->right = nodeRegistry[4];
    nodeRegistry[2]->left = nodeRegistry[5];
    nodeRegistry[2]->right = nodeRegistry[6];
}

void preorder(Node *node) {
    if (node == NULL) return;
    printf("%d - %s\n", node->id, node->content);
    preorder(node->left);
    preorder(node->right);
}

void inorder(Node *node) {
    if (node == NULL) return;
    inorder(node->left);
    printf("%d - %s\n", node->id, node->content);
    inorder(node->right);
}

void postorder(Node *node) {
    if (node == NULL) return;
    postorder(node->left);
    postorder(node->right);
    printf("%d - %s\n", node->id, node->content);
}

int findPath(Node *current, int targetId, Node *path[], int *length) {
    if (current == NULL) return 0;
    path[(*length)++] = current;
    if (current->id == targetId) return 1;
    if (findPath(current->left, targetId, path, length)) return 1;
    if (findPath(current->right, targetId, path, length)) return 1;
    (*length)--;
    return 0;
}

int height(Node *node) {
    if (node == NULL) return 0;
    int leftHeight = height(node->left);
    int rightHeight = height(node->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int countLeaves(Node *node) {
    if (node == NULL) return 0;
    if (node->left == NULL && node->right == NULL) return 1;
    return countLeaves(node->left) + countLeaves(node->right);
}

int visitTree(Node *node, unsigned char state[]) {
    int index = nodeIndex(node);
    if (index < 0 || state[index] == 1) return 0;
    if (state[index] == 2) return 1;
    state[index] = 1;
    if (node->left != NULL && !visitTree(node->left, state)) return 0;
    if (node->right != NULL && !visitTree(node->right, state)) return 0;
    state[index] = 2;
    return 1;
}

int validTree(void) {
    int parentCount[MAX_NODES] = {0};
    unsigned char state[MAX_NODES] = {0};
    int leafCount = 0;
    int decisionCount = 0;
    int rootIndex = nodeIndex(root);

    if (rootIndex < 0) return 0;
    for (int i = 0; i < nodeCount; i++) {
        Node *node = nodeRegistry[i];
        if (node->id <= 0 || node->content[0] == '\0') return 0;
        for (int j = 0; j < i; j++) {
            if (node->id == nodeRegistry[j]->id) return 0;
        }
        if (node->isLeaf == 1) {
            if (node->left != NULL || node->right != NULL) return 0;
            leafCount++;
        } else if (node->isLeaf == 0) {
            if (node->left == NULL && node->right == NULL) return 0;
            decisionCount++;
        } else {
            return 0;
        }

        Node *children[] = {node->left, node->right};
        for (int child = 0; child < 2; child++) {
            if (children[child] != NULL) {
                int childIndex = nodeIndex(children[child]);
                if (childIndex < 0 || ++parentCount[childIndex] > 1) return 0;
            }
        }
    }

    if (parentCount[rootIndex] != 0 || leafCount < 4 || decisionCount < 3)
        return 0;
    for (int i = 0; i < nodeCount; i++) {
        if (i != rootIndex && parentCount[i] != 1) return 0;
    }
    if (!visitTree(root, state)) return 0;
    for (int i = 0; i < nodeCount; i++) {
        if (state[i] != 2) return 0;
    }
    return 1;
}

int inputTree(void) {
    int total, rootId;
    int leftIds[MAX_NODES], rightIds[MAX_NODES];

    printf("Nhap so nut (7..%d): ", MAX_NODES);
    if (!readInt(&total) || total < 7 || total > MAX_NODES) {
        printf("So nut khong hop le.\n");
        return 0;
    }

    freeNodes();
    printf("Yeu cau: it nhat 4 nut la va 3 nut quyet dinh.\n");
    printf("Nut la phai khong co con; nut quyet dinh phai co it nhat 1 con.\n");
    for (int i = 0; i < total; i++) {
        int id, isLeaf;
        char content[CONTENT_SIZE];
        printf("\nNut %d:\nid (so nguyen duong): ", i + 1);
        if (!readInt(&id) || id <= 0) goto invalid_input;
        printf("Noi dung: ");
        if (fgets(content, sizeof(content), stdin) == NULL) goto invalid_input;
        if (strchr(content, '\n') == NULL && !feof(stdin)) {
            clearInput();
            goto invalid_input;
        }
        content[strcspn(content, "\r\n")] = '\0';
        printf("isLeaf (1: la, 0: quyet dinh): ");
        if (!readInt(&isLeaf) || (isLeaf != 0 && isLeaf != 1))
            goto invalid_input;
        nodeRegistry[i] = createNode(id, content, isLeaf);
        if (nodeRegistry[i] == NULL) goto invalid_input;
        nodeCount++;
    }

    for (int i = 0; i < total; i++) {
        printf("Con trai id cua nut %d (0 neu rong): ", nodeRegistry[i]->id);
        if (!readInt(&leftIds[i]) || leftIds[i] < 0) goto invalid_input;
        printf("Con phai id cua nut %d (0 neu rong): ", nodeRegistry[i]->id);
        if (!readInt(&rightIds[i]) || rightIds[i] < 0) goto invalid_input;
    }
    printf("id nut goc: ");
    if (!readInt(&rootId) || rootId <= 0) goto invalid_input;
    root = findById(rootId);
    if (root == NULL) goto invalid_input;

    for (int i = 0; i < total; i++) {
        if (leftIds[i] != 0) {
            nodeRegistry[i]->left = findById(leftIds[i]);
            if (nodeRegistry[i]->left == NULL) goto invalid_input;
        }
        if (rightIds[i] != 0) {
            nodeRegistry[i]->right = findById(rightIds[i]);
            if (nodeRegistry[i]->right == NULL) goto invalid_input;
        }
    }

    if (!validTree()) goto invalid_input;
    printf("Da nhap cay: n = %d, root id = %d.\n", nodeCount, root->id);
    return 1;

invalid_input:
    freeNodes();
    printf("Du lieu cay khong hop le. Vui long nhap lai cay.\n");
    return 0;
}

void displayTree(void) {
    printf("\nCAY CON TRO: n = %d, root id = %d\n", nodeCount, root->id);
    printf("%-6s %-6s %-35s %-8s %-8s %-7s\n",
           "id", "isLeaf", "Noi dung", "left id", "right id", "Dia chi");
    for (int i = 0; i < nodeCount; i++) {
         printf("%-6d %-6d %-35s %-8d %-8d %p\n", nodeRegistry[i]->id,
             nodeRegistry[i]->isLeaf, nodeRegistry[i]->content,
             nodeRegistry[i]->left == NULL ? 0 : nodeRegistry[i]->left->id,
             nodeRegistry[i]->right == NULL ? 0 : nodeRegistry[i]->right->id,
             (void *)nodeRegistry[i]);
    }
}

void searchAndPrintPath(void) {
    int targetId, length = 0;
    Node *path[MAX_NODES];
    printf("Nhap id can tim: ");
    if (!readInt(&targetId)) {
        printf("id khong hop le.\n");
        return;
    }
    if (!findPath(root, targetId, path, &length)) {
        printf("Khong tim thay nut.\n");
        return;
    }
    printf("Tim thay: %d - %s\nDuong di (id): ",
           path[length - 1]->id, path[length - 1]->content);
    for (int i = 0; i < length; i++) {
        printf("%d%s", path[i]->id, i < length - 1 ? " -> " : "\n");
    }
}

void runDecisionTree(void) {
    Node *current = root;
    char answer;
    while (current != NULL && !current->isLeaf) {
        printf("%s (y/n): ", current->content);
        if (scanf(" %c", &answer) != 1) return;
        current = (answer == 'y' || answer == 'Y')
            ? current->left : current->right;
    }
    if (current != NULL) printf("Ket luan: %s\n", current->content);
    else printf("Du lieu cay bi loi.\n");
}

void processChoice(int choice) {
    if (choice == 0) return;
    if (choice < 0 || choice > 7) {
        printf("Lua chon khong hop le.\n");
        return;
    }
    if (choice == 1) {
        loadSampleTree();
        if (root != NULL)
            printf("Da khoi tao cay mau: n = %d, root id = %d.\n",
                   nodeCount, root->id);
        return;
    }
    if (choice == 7) {
        inputTree();
        return;
    }
    if (root == NULL) {
        printf("Cay chua duoc khoi tao.\n");
        return;
    }
    switch (choice) {
        case 2:
            displayTree();
            break;
        case 3:
            printf("\nTIEN TU\n");
            preorder(root);
            printf("\nTRUNG TU\n");
            inorder(root);
            printf("\nHAU TU\n");
            postorder(root);
            break;
        case 4:
            searchAndPrintPath();
            break;
        case 5:
            printf("Chieu cao: %d\n", height(root));
            printf("So nut la: %d\n", countLeaves(root));
            break;
        case 6:
            runDecisionTree();
            clearInput();
            break;
    }
}

int main(void) {
    int choice;
    do {
        printf("\n===== POINTER-BASED TREE PRACTICE =====\n");
        printf("1. Khoi tao cay mau\n");
        printf("2. Hien thi cac nut va lien ket con tro\n");
        printf("3. Duyet tien tu / trung tu / hau tu\n");
        printf("4. Tim nut va in duong di\n");
        printf("5. Tinh chieu cao va dem nut la\n");
        printf("6. Mo phong tu van hoc vu\n");
        printf("7. Nhap du lieu cay thu cong\n");
        printf("0. Thoat\nChon: ");
        if (!readInt(&choice)) {
            if (feof(stdin)) break;
            printf("Lua chon khong hop le.\n");
            choice = -1;
            continue;
        }
        processChoice(choice);
    } while (choice != 0);
    freeNodes();
    return 0;
}