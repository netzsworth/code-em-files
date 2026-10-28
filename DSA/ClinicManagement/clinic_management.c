#include <stdio.h>
#include <string.h>

#define MAX_PATIENTS 100
#define NAME_LENGTH 40

struct BenhNhan {
    int ma;
    char ten[NAME_LENGTH];
    int uuTien;
};

struct Queue {
    struct BenhNhan patients[MAX_PATIENTS];
    int front;
    int rear;
    int size;
};

/* Remove the remaining characters from the current input line. */
void clearInputLine(void) {
    int character;

    while ((character = getchar()) != '\n' && character != EOF) {
        /* Discard extra input. */
    }
}

/* Read one integer from the user. */
int readInteger(const char prompt[]) {
    int value;

    while (1) {
        printf("%s", prompt);

        if (scanf("%d", &value) == 1) {
            clearInputLine();
            return value;
        }

        printf("Vui long nhap mot so nguyen.\n");
        clearInputLine();
    }
}

/* Read a non-empty patient name with a maximum length of 39 characters. */
void readName(char name[]) {
    while (1) {
        printf("Nhap ten benh nhan: ");

        if (scanf("%39[^\n]", name) == 1) {
            clearInputLine();
            return;
        }

        printf("Ten khong duoc de trong.\n");
        clearInputLine();
    }
}

/* Print all patients in the array. */
void printList(struct BenhNhan patients[], int count) {
    int i;

    if (count == 0) {
        printf("Danh sach dang rong.\n");
        return;
    }

    printf("%-5s %-10s %-8s\n", "Ma", "Ten", "UuTien");

    for (i = 0; i < count; i++) {
        printf("%-5d %-10s %-8d\n",
               patients[i].ma,
               patients[i].ten,
               patients[i].uuTien);
    }
}

/* Return 1 when the given ID is already in the list. */
int hasPatientId(struct BenhNhan patients[], int count, int wantedId) {
    int i;

    for (i = 0; i < count; i++) {
        if (patients[i].ma == wantedId) {
            return 1;
        }
    }

    return 0;
}

/* Read one patient and append it to the registration list. */
void addPatient(struct BenhNhan patients[], int *count) {
    struct BenhNhan newPatient;

    if (*count == MAX_PATIENTS) {
        printf("Danh sach da day.\n");
        return;
    }

    newPatient.ma = readInteger("Nhap ma benh nhan: ");

    if (hasPatientId(patients, *count, newPatient.ma)) {
        printf("Ma benh nhan da ton tai.\n");
        return;
    }

    readName(newPatient.ten);

    newPatient.uuTien = readInteger("Nhap muc uu tien: ");
    patients[*count] = newPatient;
    (*count)++;

    printf("Da them benh nhan.\n");
}

/* Return 1 when patient a should appear after patient b by priority order. */
int shouldComeAfterPriority(struct BenhNhan a, struct BenhNhan b) {
    if (a.uuTien > b.uuTien) {
        return 1;
    }

    if (a.uuTien == b.uuTien && a.ma > b.ma) {
        return 1;
    }

    return 0;
}

/*
 * Insertion Sort has worst-case time complexity O(n^2).
 * This happens when the array is in reverse order and many shifts are needed.
 */
void insertionSortByPriority(struct BenhNhan patients[], int count) {
    int i;
    int position;
    struct BenhNhan currentPatient;

    for (i = 1; i < count; i++) {
        currentPatient = patients[i];
        position = i - 1;

        while (position >= 0 &&
               shouldComeAfterPriority(patients[position], currentPatient)) {
            patients[position + 1] = patients[position];
            position--;
        }

        patients[position + 1] = currentPatient;
    }
}

/* Return 1 when patient a should appear after patient b by ID order. */
int shouldComeAfterId(struct BenhNhan a, struct BenhNhan b) {
    return a.ma > b.ma;
}

/*
 * Insertion Sort has worst-case time complexity O(n^2).
 * The worst case occurs when the IDs are in reverse order.
 */
void insertionSortById(struct BenhNhan patients[], int count) {
    int i;
    int position;
    struct BenhNhan currentPatient;

    for (i = 1; i < count; i++) {
        currentPatient = patients[i];
        position = i - 1;

        while (position >= 0 &&
               shouldComeAfterId(patients[position], currentPatient)) {
            patients[position + 1] = patients[position];
            position--;
        }

        patients[position + 1] = currentPatient;
    }
}

/*
 * Binary Search has worst-case time complexity O(log n).
 * Each step cuts the remaining search range approximately in half.
 */
int binarySearchById(struct BenhNhan patients[], int count, int wantedId) {
    int left = 0;
    int right = count - 1;
    int middle;

    while (left <= right) {
        middle = (left + right) / 2;

        if (patients[middle].ma == wantedId) {
            return middle;
        }

        if (patients[middle].ma < wantedId) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }

    return -1;
}

/* Set the queue to its empty state. */
void initializeQueue(struct Queue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

/* Add one patient to the rear of the queue. */
int enqueue(struct Queue *queue, struct BenhNhan patient) {
    if (queue->size == MAX_PATIENTS) {
        return 0;
    }

    queue->patients[queue->rear] = patient;
    queue->rear = (queue->rear + 1) % MAX_PATIENTS;
    queue->size++;

    return 1;
}

/* Remove one patient from the front of the queue. */
int dequeue(struct Queue *queue, struct BenhNhan *removedPatient) {
    if (queue->size == 0) {
        return 0;
    }

    *removedPatient = queue->patients[queue->front];
    queue->front = (queue->front + 1) % MAX_PATIENTS;
    queue->size--;

    return 1;
}

/* Print all patients currently waiting from front to rear. */
void printQueue(struct Queue *queue) {
    int i;
    int position;

    if (queue->size == 0) {
        printf("Hang doi dang rong.\n");
        return;
    }

    for (i = 0; i < queue->size; i++) {
        position = (queue->front + i) % MAX_PATIENTS;
        printf("%d", queue->patients[position].ma);

        if (i < queue->size - 1) {
            printf(" ");
        }
    }

    printf("\n");
}

/* Search for a patient by ID in a copy sorted by ID. */
void searchPatient(struct BenhNhan patients[], int count) {
    struct BenhNhan sortedById[MAX_PATIENTS];
    int wantedId;
    int foundIndex;

    if (count == 0) {
        printf("Danh sach dang rong.\n");
        return;
    }

    wantedId = readInteger("Nhap ma can tim: ");
    memcpy(sortedById, patients, count * sizeof(struct BenhNhan));
    insertionSortById(sortedById, count);
    foundIndex = binarySearchById(sortedById, count, wantedId);

    if (foundIndex == -1) {
        printf("KHONG TIM THAY\n");
    } else {
        printf("Tim thay: %d - %s - uu tien %d\n",
               sortedById[foundIndex].ma,
               sortedById[foundIndex].ten,
               sortedById[foundIndex].uuTien);
    }
}

/* Build a fresh queue using patients sorted by priority. */
void buildPriorityQueue(struct BenhNhan patients[], int count,
                        struct Queue *queue) {
    struct BenhNhan sortedByPriority[MAX_PATIENTS];
    int i;

    initializeQueue(queue);
    memcpy(sortedByPriority, patients, count * sizeof(struct BenhNhan));
    insertionSortByPriority(sortedByPriority, count);

    for (i = 0; i < count; i++) {
        enqueue(queue, sortedByPriority[i]);
    }

    printf("Da tao hang doi theo muc uu tien.\n");
}

/* Receive the next patient from the queue. */
void receiveNextPatient(struct Queue *queue) {
    struct BenhNhan receivedPatient;

    if (dequeue(queue, &receivedPatient)) {
        printf("Da tiep nhan: %d - %s\n",
               receivedPatient.ma,
               receivedPatient.ten);
        printf("So benh nhan dang cho: %d\n", queue->size);
    } else {
        printf("Hang doi dang rong.\n");
    }
}

/* Print the menu options. */
void printMenu(void) {
    printf("\n========== QUAN LY BENH NHAN ==========\n");
    printf("1. Them benh nhan\n");
    printf("2. Hien thi danh sach\n");
    printf("3. Sap xep theo uu tien\n");
    printf("4. Tim benh nhan theo ma\n");
    printf("5. Tao hang doi theo uu tien\n");
    printf("6. Tiep nhan benh nhan tiep theo\n");
    printf("7. Hien thi hang doi\n");
    printf("0. Thoat\n");
}

/* Run the interactive switch-case menu. */
void runMenu(void) {
    struct BenhNhan patients[MAX_PATIENTS];
    struct Queue queue;
    int patientCount = 0;
    int choice;

    initializeQueue(&queue);

    do {
        printMenu();
        choice = readInteger("Chon chuc nang: ");

        switch (choice) {
            case 1:
                addPatient(patients, &patientCount);
                break;
            case 2:
                printList(patients, patientCount);
                break;
            case 3:
                insertionSortByPriority(patients, patientCount);
                printf("Da sap xep theo uu tien.\n");
                printList(patients, patientCount);
                break;
            case 4:
                searchPatient(patients, patientCount);
                break;
            case 5:
                if (patientCount == 0) {
                    printf("Danh sach dang rong.\n");
                } else {
                    buildPriorityQueue(patients, patientCount, &queue);
                }
                break;
            case 6:
                receiveNextPatient(&queue);
                break;
            case 7:
                if (queue.size == 0) {
                    printf("Hang doi dang rong.\n");
                } else {
                    printf("Hang doi tu front den rear: ");
                    printQueue(&queue);
                }
                break;
            case 0:
                printf("Da thoat chuong trinh.\n");
                break;
            default:
                printf("Lua chon khong hop le.\n");
        }
    } while (choice != 0);
}

/* Start the clinic patient management program. */
int main(void) {
    runMenu();

    return 0;
}
