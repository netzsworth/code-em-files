#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 512
#define MAX_CODE 32
#define MAX_NAME 128
#define MAX_PRODUCTS 1000

typedef struct {
    char code[MAX_CODE];
    char name[MAX_NAME];
    double price;
    int quantity;
} Product;

static void discard_remaining_line(FILE *file) {
    int character;
    while ((character = fgetc(file)) != '\n' && character != EOF) {
    }
}

static int parse_positive_count(const char *text, int *count) {
    char *end;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') {
        end++;
    }
    if (errno != 0 || end == text || *end != '\0' || value <= 0 || value > 100000) {
        return 0;
    }
    *count = (int)value;
    return 1;
}

static int parse_product(char *line, Product *product) {
    char *fields[5];
    char *token;
    char *end;
    int field_count = 0;
    long quantity;

    token = strtok(line, "|\r\n");
    while (token != NULL && field_count < 5) {
        fields[field_count++] = token;
        token = strtok(NULL, "|\r\n");
    }
    if (field_count != 4 || token != NULL || fields[0][0] == '\0' || fields[1][0] == '\0') {
        return 0;
    }
    if (strlen(fields[0]) >= MAX_CODE || strlen(fields[1]) >= MAX_NAME) {
        return 0;
    }

    errno = 0;
    product->price = strtod(fields[2], &end);
    if (errno != 0 || end == fields[2] || *end != '\0' || product->price < 0) {
        return 0;
    }
    errno = 0;
    quantity = strtol(fields[3], &end, 10);
    if (errno != 0 || end == fields[3] || *end != '\0' || quantity < 0 || quantity > 2147483647) {
        return 0;
    }

    strcpy(product->code, fields[0]);
    strcpy(product->name, fields[1]);
    product->quantity = (int)quantity;
    return 1;
}

static int load_products(const char *filename, Product products[], int *count) {
    FILE *file;
    char line[MAX_LINE];
    int index;

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Loi: khong mo duoc tep '%s'.\n", filename);
        return 0;
    }
    if (fgets(line, sizeof(line), file) == NULL || !parse_positive_count(line, count) ||
        *count > MAX_PRODUCTS) {
        fprintf(stderr, "Loi: N trong tep khong hop le.\n");
        fclose(file);
        return 0;
    }

    for (index = 0; index < *count; index++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            fprintf(stderr, "Loi: tep co it hon N ban ghi.\n");
            fclose(file);
            return 0;
        }
        if (strchr(line, '\n') == NULL && !feof(file)) {
            discard_remaining_line(file);
            fprintf(stderr, "Loi: dong %d qua dai.\n", index + 2);
            fclose(file);
            return 0;
        }
        if (!parse_product(line, &products[index])) {
            fprintf(stderr, "Loi: ban ghi tai dong %d khong hop le.\n", index + 2);
            fclose(file);
            return 0;
        }
        for (int previous = 0; previous < index; previous++) {
            if (strcmp(products[previous].code, products[index].code) == 0) {
                fprintf(stderr, "Loi: ma san pham trung lap '%s'.\n", products[index].code);
                fclose(file);
                return 0;
            }
        }
    }
    if (fgets(line, sizeof(line), file) != NULL) {
        fprintf(stderr, "Loi: tep co nhieu hon N ban ghi.\n");
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

static void print_product(const Product *product, int position) {
    printf("  [%d] %s | %s | %.2f | %d\n", position, product->code,
           product->name, product->price, product->quantity);
}

static int sequential_search(const Product products[], int count, const char *key,
                             int *comparisons) {
    for (int index = 0; index < count; index++) {
        (*comparisons)++;
        if (strcmp(products[index].code, key) == 0) {
            return index;
        }
    }
    return -1;
}

static void insertion_sort(Product products[], int count) {
    for (int index = 1; index < count; index++) {
        Product current = products[index];
        int previous = index - 1;
        while (previous >= 0 && strcmp(products[previous].code, current.code) > 0) {
            products[previous + 1] = products[previous];
            previous--;
        }
        products[previous + 1] = current;
    }
}

static int binary_search(const Product products[], int count, const char *key,
                         int *comparisons) {
    int left = 0;
    int right = count - 1;

    while (left <= right) {
        int middle = left + (right - left) / 2;
        int result;

        (*comparisons)++;
        result = strcmp(products[middle].code, key);
        if (result == 0) {
            return middle;
        }
        if (result < 0) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }
    return -1;
}

static void print_result(const char *algorithm, const Product products[], int position,
                         int comparisons) {
    if (position < 0) {
        printf("%s: khong tim thay; so phep so sanh khoa = %d\n", algorithm, comparisons);
        return;
    }
    printf("%s: tim thay tai vi tri %d; so phep so sanh khoa = %d\n",
           algorithm, position, comparisons);
    printf("  MaSP: %s, TenSP: %s, DonGia: %.2f, SoLuong: %d\n",
           products[position].code, products[position].name,
           products[position].price, products[position].quantity);
}

int main(int argc, char *argv[]) {
    Product original[MAX_PRODUCTS];
    Product sorted[MAX_PRODUCTS];
    int count;
    int sequential_comparisons = 0;
    int binary_comparisons = 0;
    int sequential_position;
    int binary_position;

    if (argc != 3) {
        fprintf(stderr, "Cach dung: %s <san_pham.txt> <ma_san_pham>\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (!load_products(argv[1], original, &count)) {
        return EXIT_FAILURE;
    }

    printf("Danh sach san pham (%d ban ghi):\n", count);
    for (int index = 0; index < count; index++) {
        print_product(&original[index], index);
    }

    for (int index = 0; index < count; index++) {
        sorted[index] = original[index];
    }
    insertion_sort(sorted, count);

    sequential_position = sequential_search(original, count, argv[2], &sequential_comparisons);
    binary_position = binary_search(sorted, count, argv[2], &binary_comparisons);
    printf("\nKhoa can tim: %s\n", argv[2]);
    print_result("Tim tuan tu tren mang goc", original, sequential_position, sequential_comparisons);
    print_result("Tim nhi phan tren ban sao da sap xep", sorted, binary_position, binary_comparisons);

    return EXIT_SUCCESS;
}