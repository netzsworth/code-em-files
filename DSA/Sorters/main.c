#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "student.c"
#include "sorts.c"

typedef enum { ALG_INSERTION = 1, ALG_SELECTION, ALG_BUBBLE, ALG_QUICK } Alg;
typedef enum { CRIT_MAIN = 1, CRIT_GPA } Crit;

static const char *alg_name(Alg a) {
    if (a == ALG_INSERTION) {
        return "Insertion Sort";
    }
    if (a == ALG_SELECTION) {
        return "Selection Sort";
    }
    if (a == ALG_BUBBLE) {
        return "Bubble Sort";
    }
    return "Quick Sort";
}

static const char *crit_name(Crit c) {
    if (c == CRIT_MAIN) {
        return "Xep hang chinh (GPA giam, tin chi giam, ma SV tang)";
    }
    return "Chi GPA giam dan";
}

static CompareFn crit_fn(Crit c) {
    if (c == CRIT_MAIN) {
        return cmp_main;
    }
    return cmp_gpa_only;
}

static void run_sort(Alg a, Student *arr, int n, CompareFn cmp, SortStats *st) {
    if (a == ALG_INSERTION) {
        insertion_sort(arr, n, cmp, st);
    } else if (a == ALG_SELECTION) {
        selection_sort(arr, n, cmp, st);
    } else if (a == ALG_BUBBLE) {
        bubble_sort(arr, n, cmp, st);
    } else {
        quick_sort(arr, n, cmp, st);
    }
}

/* ---- trang thai chuong trinh ---- */
static char       g_path[256] = "students.csv";
static StudentList g_all, g_eligible, g_ranked;
static RejectList  g_invalid, g_ineligible;
static int g_dataLines = 0;
static int g_loaded = 0;
static int g_filtered = 0;
static int g_sorted = 0;
static Crit g_crit = CRIT_MAIN;
static Alg g_alg = ALG_INSERTION;
static SortStats g_stats;
static Alg  g_lastAlg;
static Crit g_lastCrit;
static int g_lastStable = 1;
static int g_lastSorted = 1;

static int read_int(const char *prompt) {
    char buf[64];
    printf("%s", prompt);
    if (!fgets(buf, sizeof buf, stdin)) {
        return -1;
    }

    char *end;
    long v = strtol(buf, &end, 10);
    if (end == buf) {
        return -1;
    }
    return (int)v;
}

static void print_student_row(int rank, const Student *s) {
    printf("  %-5d %-8s %-14s %5.2f %7d %5d  (nhap #%d)\n",
           rank, s->id, s->name, s->gpa, s->credits, s->discipline, s->inputIndex);
}

/* F1 */
static void do_load(void) {
    char buf[256];
    printf("Duong dan CSV [%s]: ", g_path);
    if (fgets(buf, sizeof buf, stdin)) {
        buf[strcspn(buf, "\r\n")] = '\0';
        if (buf[0]) {
            snprintf(g_path, sizeof g_path, "%s", buf);
        }
    }
    if (load_csv(g_path, &g_all, &g_invalid, &g_dataLines) != 0) {
        printf("Khong mo duoc file '%s'.\n", g_path);
        g_loaded = g_filtered = g_sorted = 0;
        return;
    }
    g_loaded = 1;
    g_filtered = 0;
    g_sorted = 0;
    printf("Da doc %d dong du lieu: %d hop le ve dinh dang, %d loi.\n",
           g_dataLines, g_all.size, g_invalid.size);
    for (int i = 0; i < g_invalid.size; i++) {
        const char *id = g_invalid.data[i].id;
        if (id[0] == '\0') {
            id = "?";
        }
        printf("  [Loi] dong %d (%s): %s\n", g_invalid.data[i].line,
               id, g_invalid.data[i].reason);
    }
}

/* F2 */
static int ensure_filtered(void) {
    if (!g_loaded) {
        printf("Hay doc CSV truoc (muc 1).\n");
        return 0;
    }
    if (!g_filtered) {
        filter_eligible(&g_all, &g_eligible, &g_ineligible);
        g_filtered = 1;
        g_sorted = 0;
    }
    return 1;
}
static void do_filter(void) {
    if (!g_loaded) {
        printf("Hay doc CSV truoc (muc 1).\n");
        return;
    }
    filter_eligible(&g_all, &g_eligible, &g_ineligible);
    g_filtered = 1;
    g_sorted = 0;
    printf("Du dieu kien: %d | Khong du dieu kien: %d\n", g_eligible.size, g_ineligible.size);
    for (int i = 0; i < g_ineligible.size; i++) {
        printf("  [Loai] %s: %s\n", g_ineligible.data[i].id, g_ineligible.data[i].reason);
    }
}

/* F4 */
static void do_choose_crit(void) {
    printf("1. %s\n2. %s\n", crit_name(CRIT_MAIN), crit_name(CRIT_GPA));
    int c = read_int("Chon tieu chi: ");
    if (c == 1 || c == 2) {
        g_crit = (Crit)c;
    } else {
        printf("Lua chon khong hop le.\n");
    }
    printf("Tieu chi hien tai: %s\n", crit_name(g_crit));
}

/* F3 */
static void do_sort(void) {
    if (!ensure_filtered()) {
        return;
    }
    printf("1. Insertion Sort\n2. Selection Sort\n3. Bubble Sort\n4. Quick Sort\n");
    int a = read_int("Chon thuat toan: ");
    if (a < 1 || a > 4) {
        printf("Lua chon khong hop le.\n");
        return;
    }
    g_alg = (Alg)a;

    /* luon sao chep tu du lieu goc de co the chay lai nhieu thuat toan */
    if (sl_copy(&g_ranked, &g_eligible) != 0) {
        printf("Het bo nho.\n");
        return;
    }
    CompareFn cmp = crit_fn(g_crit);
    run_sort(g_alg, g_ranked.data, g_ranked.size, cmp, &g_stats);
    g_sorted = 1;
    g_lastAlg = g_alg;
    g_lastCrit = g_crit;
    g_lastSorted = is_sorted(g_ranked.data, g_ranked.size, cmp);

    const Student *b1 = NULL, *b2 = NULL;
    g_lastStable = is_stable(g_ranked.data, g_ranked.size, cmp, &b1, &b2);

    printf("%s | %s\n", alg_name(g_alg), crit_name(g_crit));
    printf("  So sanh: %ld | Di chuyen: %ld | Thoi gian: %.6f s\n",
           g_stats.comparisons, g_stats.moves, g_stats.seconds);
    const char *sorted_status = "SAI";
    if (g_lastSorted) {
        sorted_status = "DUNG";
    }
    const char *stable_status = "KHONG";
    if (g_lastStable) {
        stable_status = "CO";
    }
    printf("  Dung thu tu: %s | On dinh: %s\n", sorted_status, stable_status);
    if (!g_lastStable) {
        printf("  Vi pham on dinh: %s (nhap #%d) dung truoc %s (nhap #%d) du bang khoa.\n",
               b1->id, b1->inputIndex, b2->id, b2->inputIndex);
    }
}

/* F5 */
static void do_topk(void) {
    if (!g_sorted) {
        printf("Hay xep hang truoc (muc 4).\n");
        return;
    }
    int k = read_int("Nhap K: ");
    if (k < 0) {
        printf("K khong hop le.\n");
        return;
    }
    if (k == 0) {
        printf("K = 0: khong co sinh vien nao de hien thi.\n");
    } else {
        int show = k;
        if (k > g_ranked.size) {
            show = g_ranked.size;
        }
        if (k > g_ranked.size)
            printf("K = %d lon hon so hop le (%d): chi in %d sinh vien.\n", k, g_ranked.size, show);
        printf("  Hang  Ma SV    Ho ten           GPA  TinChi KyLuat\n");
        for (int i = 0; i < show; i++) {
            print_student_row(i + 1, &g_ranked.data[i]);
        }
    }
    printf("Thong ke: tong %d | loi dinh dang %d | khong du dieu kien %d | du dieu kien %d\n",
           g_dataLines, g_invalid.size, g_ineligible.size, g_eligible.size);
}

/* So sanh ca 3 thuat toan tren cung du lieu goc */
static void do_compare_all(void) {
    if (!ensure_filtered()) {
        return;
    }
    CompareFn cmp = crit_fn(g_crit);
    printf("Tieu chi: %s | n = %d\n", crit_name(g_crit), g_eligible.size);
    printf("  %-16s %12s %12s %12s %8s %8s\n", "Thuat toan", "So sanh", "Di chuyen", "Thoi gian(s)", "Dung", "On dinh");
    for (int a = 1; a <= 4; a++) {
        StudentList tmp;
        sl_init(&tmp);
        if (sl_copy(&tmp, &g_eligible) != 0) {
            printf("Het bo nho.\n");
            sl_free(&tmp);
            continue;
        }
        SortStats st;
        run_sort((Alg)a, tmp.data, tmp.size, cmp, &st);
        const char *sorted_status = "SAI";
        if (is_sorted(tmp.data, tmp.size, cmp)) {
            sorted_status = "DUNG";
        }
        const char *stable_status = "KHONG";
        if (is_stable(tmp.data, tmp.size, cmp, NULL, NULL)) {
            stable_status = "CO";
        }
        printf("  %-16s %12ld %12ld %12.6f %8s %8s\n", alg_name((Alg)a), st.comparisons, st.moves,
               st.seconds, sorted_status, stable_status);
        sl_free(&tmp);
    }
}

/* F6 */
static void do_write(void) {
    if (!g_sorted) {
        printf("Hay xep hang truoc (muc 4).\n");
        return;
    }
    int ok = 1;
    ok &= write_valid_csv("valid.csv", &g_eligible) == 0;
    ok &= write_rejected_csv("rejected.csv", &g_invalid, &g_ineligible) == 0;
    ok &= write_ranked_csv("ranked.csv", &g_ranked) == 0;

    FILE *f = fopen("summary.txt", "w");
    if (f) {
        time_t t = time(NULL);
        fprintf(f, "BAO CAO XET HOC BONG\n");
        fprintf(f, "Thoi diem chay      : %s", ctime(&t));
        fprintf(f, "Tep dau vao         : %s\n", g_path);
        fprintf(f, "Tong so dong du lieu: %d\n", g_dataLines);
        fprintf(f, "Loi dinh dang       : %d\n", g_invalid.size);
        fprintf(f, "Hop le ve dinh dang : %d\n", g_all.size);
        fprintf(f, "Khong du dieu kien  : %d\n", g_ineligible.size);
        fprintf(f, "Du dieu kien        : %d\n", g_eligible.size);
        fprintf(f, "Thuat toan          : %s\n", alg_name(g_lastAlg));
        fprintf(f, "Tieu chi            : %s\n", crit_name(g_lastCrit));
        fprintf(f, "So lan so sanh      : %ld\n", g_stats.comparisons);
        fprintf(f, "So lan di chuyen    : %ld\n", g_stats.moves);
        fprintf(f, "Thoi gian (s)       : %.6f\n", g_stats.seconds);
        const char *sorted_status = "SAI";
        if (g_lastSorted) {
            sorted_status = "DUNG";
        }
        const char *stable_status = "KHONG";
        if (g_lastStable) {
            stable_status = "CO";
        }
        fprintf(f, "Dung thu tu         : %s\n", sorted_status);
        fprintf(f, "On dinh             : %s\n", stable_status);
        fclose(f);
    } else {
        ok = 0;
    }
    if (ok) {
        printf("Da ghi valid.csv, rejected.csv, ranked.csv, summary.txt\n");
    } else {
        printf("Loi khi ghi file.\n");
    }
}

static void print_menu(void) {
    printf("\n===== QUAN LY XET HOC BONG =====\n");
    printf("1. Doc CSV va kiem tra (F1)\n");
    printf("2. Loc sinh vien du dieu kien (F2)\n");
    printf("3. Chon tieu chi xep hang (F4)    [hien tai: %s]\n", crit_name(g_crit));
    printf("4. Xep hang bang thuat toan (F3)\n");
    printf("5. Hien thi Top K va thong ke (F5)\n");
    printf("6. Ghi ranked.csv, valid.csv, rejected.csv, summary.txt (F6)\n");
    printf("7. So sanh ca 4 thuat toan\n");
    printf("0. Thoat\n");
}

int main(int argc, char **argv) {
    if (argc > 1) {
        snprintf(g_path, sizeof g_path, "%s", argv[1]);
    }
    sl_init(&g_all);
    sl_init(&g_eligible);
    sl_init(&g_ranked);
    rl_init(&g_invalid);
    rl_init(&g_ineligible);

    for (;;) {
        print_menu();
        int c = read_int("Chon: ");
        if (c == 0 || c == -1) {
            break;
        }
        switch (c) {
            case 1:
                do_load();
                break;
            case 2:
                do_filter();
                break;
            case 3:
                do_choose_crit();
                break;
            case 4:
                do_sort();
                break;
            case 5:
                do_topk();
                break;
            case 6:
                do_write();
                break;
            case 7:
                do_compare_all();
                break;
            default:
                printf("Lua chon khong hop le.\n");
        }
    }
    sl_free(&g_all);
    sl_free(&g_eligible);
    sl_free(&g_ranked);
    rl_free(&g_invalid);
    rl_free(&g_ineligible);
    return 0;
}
