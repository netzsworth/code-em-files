#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 49

typedef struct Product {
    int maSP;
    char tenSP[50];
    int soLuong;
    double donGia;
} Product;

typedef struct Node {
    Product product;
    struct Node *left;
    struct Node *right;
} Node;

/* Remove the rest of the current input line. */
void discard_line(void) {
    int character;

    while ((character = getchar()) != '\n' && character != EOF) {
    }
}

/* Check whether a product satisfies all validation rules. */
int is_valid_product(Product product) {
    return product.maSP > 0 && product.soLuong >= 0 && product.donGia >= 0.0 &&
           product.tenSP[0] != '\0';
}

/* Allocate and initialize one tree node. */
Node *create_node(Product product) {
    Node *node = malloc(sizeof(Node));

    if (node == NULL) {
        printf("Loi: khong du bo nho de tao nut\n");
        return NULL;
    }

    node->product = product;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* Insert a product into the BST, or report a duplicate code. */
Node *insert_product(Node *root, Product product) {
    Node *new_node;

    if (!is_valid_product(product)) {
        printf("Du lieu san pham khong hop le\n");
        return root;
    }

    if (root == NULL) {
        return create_node(product);
    }

    if (product.maSP == root->product.maSP) {
        printf("Mã sản phẩm đã tồn tại\n");
    } else if (product.maSP < root->product.maSP) {
        new_node = insert_product(root->left, product);
        if (new_node != root->left && new_node != NULL) {
            root->left = new_node;
        }
    } else {
        new_node = insert_product(root->right, product);
        if (new_node != root->right && new_node != NULL) {
            root->right = new_node;
        }
    }

    return root;
}

/* Find a product by its product code. */
Node *search_product(Node *root, int maSP) {
    if (root == NULL || root->product.maSP == maSP) {
        return root;
    }

    if (maSP < root->product.maSP) {
        return search_product(root->left, maSP);
    }
    return search_product(root->right, maSP);
}

/* Print one product with all its information. */
void print_product(Product product) {
    printf("MaSP: %d | TenSP: %s | SoLuong: %d | DonGia: %.2f\n",
           product.maSP, product.tenSP, product.soLuong, product.donGia);
}

/* Print all products in ascending product-code order. */
void print_in_order(Node *root) {
    if (root == NULL) {
        return;
    }

    print_in_order(root->left);
    printf("%-8d %-30s %-10d %-12.2f\n",
           root->product.maSP, root->product.tenSP,
           root->product.soLuong, root->product.donGia);
    print_in_order(root->right);
}

/* Print the table header and all products. */
void print_product_list(Node *root) {
    if (root == NULL) {
        printf("Cay dang rong\n");
        return;
    }

    printf("%-8s %-30s %-10s %-12s\n",
           "MaSP", "TenSP", "SoLuong", "DonGia");
    printf("------------------------------------------------------------\n");
    print_in_order(root);
}

/* Find the node with the smallest code in a non-empty subtree. */
Node *find_smallest(Node *root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

/* Delete one node recursively and record whether a node was removed. */
Node *delete_node(Node *root, int maSP, int *deleted) {
    Node *smallest;

    if (root == NULL) {
        return NULL;
    }

    if (maSP < root->product.maSP) {
        root->left = delete_node(root->left, maSP, deleted);
    } else if (maSP > root->product.maSP) {
        root->right = delete_node(root->right, maSP, deleted);
    } else {
        *deleted = 1;
        if (root->left == NULL) {
            Node *right_child = root->right;
            free(root);
            return right_child;
        }
        if (root->right == NULL) {
            Node *left_child = root->left;
            free(root);
            return left_child;
        }

        smallest = find_smallest(root->right);
        root->product = smallest->product;
        root->right = delete_node(root->right, smallest->product.maSP, deleted);
    }

    return root;
}

/* Delete a product from the BST and report when its code is absent. */
void delete_product(Node **root, int maSP) {
    int deleted = 0;

    *root = delete_node(*root, maSP, &deleted);
    if (!deleted) {
        printf("Không tìm thấy sản phẩm\n");
    }
}

/* Count the number of nodes in the tree. */
int count_nodes(Node *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

/* Calculate the height as the number of nodes on the longest path. */
int tree_height(Node *root) {
    int left_height;
    int right_height;

    if (root == NULL) {
        return 0;
    }

    left_height = tree_height(root->left);
    right_height = tree_height(root->right);
    if (left_height > right_height) {
        return left_height + 1;
    }
    return right_height + 1;
}

/* Free every node in post-order. */
void free_tree(Node *root) {
    if (root == NULL) {
        return;
    }

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* Read one product from the keyboard. */
int read_product(Product *product) {
    printf("MaSP: ");
    if (scanf("%d", &product->maSP) != 1) {
        discard_line();
        printf("Du lieu san pham khong hop le\n");
        return 0;
    }
    discard_line();

    printf("TenSP: ");
    if (scanf(" %49[^\n]", product->tenSP) != 1) {
        discard_line();
        printf("Du lieu san pham khong hop le\n");
        return 0;
    }
    discard_line();

    printf("SoLuong: ");
    if (scanf("%d", &product->soLuong) != 1) {
        discard_line();
        printf("Du lieu san pham khong hop le\n");
        return 0;
    }
    discard_line();

    printf("DonGia: ");
    if (scanf("%lf", &product->donGia) != 1) {
        discard_line();
        printf("Du lieu san pham khong hop le\n");
        return 0;
    }
    discard_line();

    if (!is_valid_product(*product)) {
        printf("Du lieu san pham khong hop le\n");
        return 0;
    }
    return 1;
}

/* Insert the required sample products in the specified order. */
void load_sample_data(Node **root) {
    Product samples[] = {
        {50, "Bút bi", 120, 5000},
        {30, "Vở ô ly", 80, 12000},
        {70, "Bìa hồ sơ", 35, 15000},
        {20, "Thước kẻ", 40, 8000},
        {40, "Tẩy", 65, 4000},
        {60, "Sổ tay", 25, 25000},
        {80, "Bút chì", 110, 3000}
    };
    int sample_count = (int)(sizeof(samples) / sizeof(samples[0]));
    int index;

    free_tree(*root);
    *root = NULL;
    for (index = 0; index < sample_count; index++) {
        *root = insert_product(*root, samples[index]);
    }
    printf("Da nap du lieu mau\n");
}

/* Print the program menu. */
void print_menu(void) {
    printf("\n===== QUAN LY CATALOG SAN PHAM =====\n");
    printf("1. Them san pham\n");
    printf("2. Tim san pham theo maSP\n");
    printf("3. Xoa san pham theo maSP\n");
    printf("4. In danh sach san pham\n");
    printf("5. Dem so nut va chieu cao cay\n");
    printf("6. Nap du lieu mau\n");
    printf("0. Thoat\n");
}

/* Run the menu until the user chooses to exit. */
void run_menu(Node **root) {
    int choice;
    int maSP;
    Product product;
    Node *found;

    do {
        print_menu();
        printf("Chon chuc nang: ");
        if (scanf("%d", &choice) != 1) {
            discard_line();
            printf("Lua chon khong hop le\n");
            continue;
        }
        discard_line();

        switch (choice) {
            case 1:
                if (read_product(&product)) {
                    *root = insert_product(*root, product);
                }
                break;
            case 2:
                printf("Nhap maSP can tim: ");
                if (scanf("%d", &maSP) != 1) {
                    discard_line();
                    printf("Ma san pham khong hop le\n");
                    break;
                }
                discard_line();
                found = search_product(*root, maSP);
                if (found == NULL) {
                    printf("Không tìm thấy sản phẩm\n");
                } else {
                    print_product(found->product);
                }
                break;
            case 3:
                printf("Nhap maSP can xoa: ");
                if (scanf("%d", &maSP) != 1) {
                    discard_line();
                    printf("Ma san pham khong hop le\n");
                    break;
                }
                discard_line();
                delete_product(root, maSP);
                break;
            case 4:
                print_product_list(*root);
                break;
            case 5:
                printf("So nut: %d\n", count_nodes(*root));
                printf("Chieu cao cay: %d\n", tree_height(*root));
                break;
            case 6:
                load_sample_data(root);
                break;
            case 0:
                printf("Ket thuc chuong trinh\n");
                break;
            default:
                printf("Lua chon khong hop le\n");
        }
    } while (choice != 0);
}

/* Start the product catalog program and free the tree before exiting. */
int main(void) {
    Node *root = NULL;

    run_menu(&root);
    free_tree(root);
    return 0;
}
