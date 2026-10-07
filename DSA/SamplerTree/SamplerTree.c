#include <stdio.h>
#include <string.h>

#define MAX 100
#define EMPTY 0

typedef struct {
    int id;                 // mã nút
    char content[120];      // câu hỏi hoặc kết luận
    int left;               // chỉ số nút con trái
    int right;              // chỉ số nút con phải
    int isLeaf;             // 1: nút lá, 0: nút quyết định
} Node;

Node Tree[MAX];
int root = 1;
int n = 0;

void loadSampleTree() {
    n = 7; root = 1;

    Tree[1] = (Node){1, "DTB < 2.0?", 2, 3, 0};
    Tree[2] = (Node){2, "No >= 12 tin chi?", 4, 5, 0};
    Tree[3] = (Node){3, "DTB >= 3.2?", 6, 7, 0};
    Tree[4] = (Node){4, "Canh bao hoc vu muc 2", 0, 0, 1};
    Tree[5] = (Node){5, "Canh bao hoc vu muc 1", 0, 0, 1};
    Tree[6] = (Node){6, "De xuat khen thuong", 0, 0, 1};
    Tree[7] = (Node){7, "Theo doi binh thuong", 0, 0, 1};
}

void preorder(int i) {
    if (i == EMPTY) return;
    printf("%d - %s\n", Tree[i].id, Tree[i].content);
    preorder(Tree[i].left);
    preorder(Tree[i].right);
}

void inorder(int i) {
    if (i == EMPTY) return;
    inorder(Tree[i].left);
    printf("%d - %s\n", Tree[i].id, Tree[i].content);
    inorder(Tree[i].right);
}

void postorder(int i) {
    if (i == EMPTY) return;
    postorder(Tree[i].left);
    postorder(Tree[i].right);
    printf("%d - %s\n", Tree[i].id, Tree[i].content);
}

int findPath(int current, int targetId, int path[], int *len) {
    if (current == EMPTY) return 0;

    path[(*len)++] = current;

    if (Tree[current].id == targetId) return 1;

    if (findPath(Tree[current].left, targetId, path, len)) return 1;
    if (findPath(Tree[current].right, targetId, path, len)) return 1;

    (*len)--;       // quay lui khi nhánh hiện tại không chứa nút cần tìm
    return 0;
}

int height(int i) {
    if (i == EMPTY) return 0;
    int hLeft = height(Tree[i].left);
    int hRight = height(Tree[i].right);
    return 1 + (hLeft > hRight ? hLeft : hRight);
}

int countLeaves(int i) {
    if (i == EMPTY) return 0;
    if (Tree[i].left == EMPTY && Tree[i].right == EMPTY) return 1;
    return countLeaves(Tree[i].left) + countLeaves(Tree[i].right);
}

void runDecisionTree() {
    int current = root;
    char answer;

    while (current != EMPTY && Tree[current].isLeaf == 0) {
        printf("%s (y/n): ", Tree[current].content);
        scanf(" %c", &answer);
        if (answer == 'y' || answer == 'Y')
            current = Tree[current].left;
        else
            current = Tree[current].right;
    }

    if (current != EMPTY)
        printf("Ket luan: %s\n", Tree[current].content);
    else
        printf("Du lieu cay bi loi.\n");
}

/* Doc du lieu tu ban phim. */
void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int readInt(int *value) {
    char line[120], extra;
    if (fgets(line, sizeof(line), stdin) == NULL) return 0;
    if (strchr(line, '\n') == NULL && !feof(stdin)) {
        clearInput();
        return 0;
    }
    return sscanf(line, "%d %c", value, &extra) == 1;
}

/* Danh dau cac nut de phat hien lien ket lap hoac chu trinh. */
int visitTree(int index, int visited[]) {
    if (index == EMPTY) return 1;
    if (visited[index] == 1) return 0;

    visited[index] = 1;
    if (!visitTree(Tree[index].left, visited)) return 0;
    if (!visitTree(Tree[index].right, visited)) return 0;
    return 1;
}

/* Kiem tra du lieu va bao dam moi nut deu thuoc cay co goc 1. */
int validTree(void) {
    int visited[MAX] = {0};
    int leafCount = 0;
    int decisionCount = 0;

    for (int i = 1; i <= n; i++) {
        int left = Tree[i].left;
        int right = Tree[i].right;

        if (left < 0 || left > n || right < 0 || right > n) return 0;

        if (Tree[i].isLeaf == 1) {
            if (left != EMPTY || right != EMPTY) return 0;
            leafCount++;
        } else if (Tree[i].isLeaf == 0) {
            if (left == EMPTY && right == EMPTY) return 0;
            decisionCount++;
        } else {
            return 0;
        }

        for (int j = 1; j < i; j++) {
            if (Tree[i].id == Tree[j].id) return 0;
        }
    }

    if (leafCount < 4 || decisionCount < 3) return 0;
    if (!visitTree(root, visited)) return 0;

    for (int i = 1; i <= n; i++) {
        if (visited[i] == 0) return 0;
    }
    return 1;
}

/* Nhap cac truong cua mot nut tai vi tri index. */
int inputNode(int index) {
    printf("Nut tai index %d:\n", index);
    printf("id: ");
    if (!readInt(&Tree[index].id)) return 0;

    printf("Noi dung: ");
    if (fgets(Tree[index].content, sizeof(Tree[index].content), stdin) == NULL)
        return 0;
    if (strchr(Tree[index].content, '\n') == NULL && !feof(stdin)) {
        clearInput();
        return 0;
    }
    Tree[index].content[strcspn(Tree[index].content, "\r\n")] = '\0';
    if (Tree[index].content[0] == '\0') return 0;

    printf("left (0..%d): ", n);
    if (!readInt(&Tree[index].left)) return 0;
    if (Tree[index].left < 0 || Tree[index].left > n) return 0;

    printf("right (0..%d): ", n);
    if (!readInt(&Tree[index].right)) return 0;
    if (Tree[index].right < 0 || Tree[index].right > n) return 0;

    printf("isLeaf (1: la, 0: quyet dinh): ");
    if (!readInt(&Tree[index].isLeaf)) return 0;
    return 1;
}

void inputTree(void) {
    int total;
    int inputOK = 1;

    printf("Nhap so nut (7..%d): ", MAX - 1);
    if (!readInt(&total) || total < 7 || total >= MAX) {
        printf("So nut khong hop le.\n");
        return;
    }

    memset(Tree, 0, sizeof(Tree));
    n = total;
    root = 1;

    for (int i = 1; i <= n; i++) {
        if (!inputNode(i)) {
            inputOK = 0;
            break;
        }
    }

    if (inputOK && validTree()) {
        printf("Da nhap cay: n = %d, root = %d.\n", n, root);
    } else {
        memset(Tree, 0, sizeof(Tree));
        n = 0;
        printf("Du lieu cay khong hop le. Vui long nhap lai cay.\n");
    }
}

/* Hien thi mang va ket qua tim duong di. */
void displayTree(void) {
    printf("\nBANG MANG: n = %d, root = %d\n", n, root);
    printf("%-6s %-6s %-35s %-6s %-6s\n", "Index", "id", "Noi dung", "left", "right");
    for (int i = 1; i <= n; i++)
        printf("%-6d %-6d %-35s %-6d %-6d\n", i, Tree[i].id,
               Tree[i].content, Tree[i].left, Tree[i].right);
}

void searchAndPrintPath(void) {
    int targetId, path[MAX], len = 0;
    printf("Nhap id can tim: ");
    if (!readInt(&targetId)) {
        printf("id khong hop le.\n");
        return;
    }
    if (!findPath(root, targetId, path, &len)) {
        printf("Khong tim thay nut.\n");
        return;
    }
    int index = path[len - 1];
    printf("Tim thay tai index %d: %d - %s\n", index, Tree[index].id, Tree[index].content);
    printf("Duong di (id): ");
    for (int i = 0; i < len; i++) {
        printf("%d", Tree[path[i]].id);
        if (i < len - 1) printf(" -> ");
    }
    printf("\n");
}

void processChoice(int choice) {
    if (choice == 0) return;
    if (choice < 0 || choice > 7) {
        printf("Lua chon khong hop le.\n");
        return;
    }
    if (choice == 1) {
        memset(Tree, 0, sizeof(Tree));
        loadSampleTree();
        printf("Da khoi tao cay mau: n = %d, root = %d.\n", n, root);
        return;
    }
    if (choice == 7) {
        inputTree();
        return;
    }
    if (n == 0) {
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

int main() {
    int choice;
    do {
        printf("\n===== TREE ARRAY PRACTICE =====\n");
        printf("1. Khoi tao cay mau\n");
        printf("2. Hien thi bang mang\n");
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
    return 0;
}
