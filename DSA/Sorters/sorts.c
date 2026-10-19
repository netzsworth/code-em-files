#include <string.h>
#include <time.h>

#ifndef SORTERS_STUDENT_TYPES
#define SORTERS_STUDENT_TYPES

typedef struct {
    char id[32];
    char name[64];
    double gpa;
    int credits;
    int discipline;
    int inputIndex;
} Student;

#endif

typedef int (*CompareFn)(const Student *a, const Student *b);

int cmp_main(const Student *a, const Student *b);
int cmp_gpa_only(const Student *a, const Student *b);

typedef struct {
    long comparisons;
    long moves;
    double seconds;
} SortStats;

void insertion_sort(Student *a, int n, CompareFn cmp, SortStats *st);
void selection_sort(Student *a, int n, CompareFn cmp, SortStats *st);
void bubble_sort(Student *a, int n, CompareFn cmp, SortStats *st);
void quick_sort(Student *a, int n, CompareFn cmp, SortStats *st);
int is_sorted(const Student *a, int n, CompareFn cmp);
int is_stable(const Student *a, int n, CompareFn cmp,
             const Student **bad1, const Student **bad2);

int cmp_main(const Student *a, const Student *b) {
    if (a->gpa > b->gpa) {
        return -1;
    }
    if (a->gpa < b->gpa) {
        return 1;
    }
    if (a->credits > b->credits) {
        return -1;
    }
    if (a->credits < b->credits) {
        return 1;
    }
    return strcmp(a->id, b->id);
}

int cmp_gpa_only(const Student *a, const Student *b) {
    if (a->gpa > b->gpa) {
        return -1;
    }
    if (a->gpa < b->gpa) {
        return 1;
    }
    return 0;
}

static double now_sec(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}

/* Insertion Sort - on dinh: chi dich chuyen khi cmp(key, a[j]) < 0 (nghiem ngat) */
void insertion_sort(Student *a, int n, CompareFn cmp, SortStats *st) {
    st->comparisons = st->moves = 0;
    double t0 = now_sec();
    for (int i = 1; i < n; i++) {
        Student key = a[i];
        st->moves++;
        int j = i - 1;
        while (j >= 0) {
            st->comparisons++;
            if (cmp(&key, &a[j]) < 0) {
                a[j + 1] = a[j];
                st->moves++;
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = key;
        st->moves++;
    }
    st->seconds = now_sec() - t0;
}

/* Selection Sort - KHONG on dinh do hoan doi xa (swap) */
void selection_sort(Student *a, int n, CompareFn cmp, SortStats *st) {
    st->comparisons = st->moves = 0;
    double t0 = now_sec();
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            st->comparisons++;
            if (cmp(&a[j], &a[best]) < 0) {
                best = j;
            }
        }
        if (best != i) {
            Student t = a[i];
            a[i] = a[best];
            a[best] = t;
            st->moves += 3;
        }
    }
    st->seconds = now_sec() - t0;
}

/* Bubble Sort co co hieu dung som - on dinh: chi doi cho khi cmp(a[j+1], a[j]) < 0 */
void bubble_sort(Student *a, int n, CompareFn cmp, SortStats *st) {
    st->comparisons = st->moves = 0;
    double t0 = now_sec();
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            st->comparisons++;
            if (cmp(&a[j + 1], &a[j]) < 0) {
                Student t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
                st->moves += 3;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
    st->seconds = now_sec() - t0;
}

static int quick_partition(Student *a, int low, int high, CompareFn cmp, SortStats *st) {
    Student pivot = a[high];
    st->moves++;
    int i = low;
    for (int j = low; j < high; j++) {
        st->comparisons++;
        if (cmp(&a[j], &pivot) <= 0) {
            Student t = a[i];
            a[i] = a[j];
            a[j] = t;
            st->moves += 3;
            i++;
        }
    }
    Student t = a[i];
    a[i] = a[high];
    a[high] = t;
    st->moves += 3;
    return i;
}

static void quick_sort_range(Student *a, int low, int high, CompareFn cmp, SortStats *st) {
    if (low >= high) {
        return;
    }
    int pivot = quick_partition(a, low, high, cmp, st);
    quick_sort_range(a, low, pivot - 1, cmp, st);
    quick_sort_range(a, pivot + 1, high, cmp, st);
}

void quick_sort(Student *a, int n, CompareFn cmp, SortStats *st) {
    st->comparisons = st->moves = 0;
    double t0 = now_sec();
    quick_sort_range(a, 0, n - 1, cmp, st);
    st->seconds = now_sec() - t0;
}

int is_sorted(const Student *a, int n, CompareFn cmp) {
    for (int i = 0; i + 1 < n; i++) {
        if (cmp(&a[i], &a[i + 1]) > 0) {
            return 0;
        }
    }
    return 1;
}

int is_stable(const Student *a, int n, CompareFn cmp, const Student **b1, const Student **b2) {
    for (int i = 0; i + 1 < n; i++) {
        if (cmp(&a[i], &a[i + 1]) == 0
                && a[i].inputIndex > a[i + 1].inputIndex) {
            if (b1) {
                *b1 = &a[i];
            }
            if (b2) {
                *b2 = &a[i + 1];
            }
            return 0;
        }
    }
    return 1;
}
