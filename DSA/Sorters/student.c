#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define ID_LEN     32
#define NAME_LEN   64
#define REASON_LEN 160
#define MIN_GPA    3.20
#define MIN_CREDITS 24

#ifndef SORTERS_STUDENT_TYPES
#define SORTERS_STUDENT_TYPES

typedef struct {
    char   id[ID_LEN];
    char   name[NAME_LEN];
    double gpa;
    int    credits;
    int    discipline;
    int    inputIndex;
} Student;

typedef struct {
    int line;
    int isInvalid;
    char id[ID_LEN];
    char reason[REASON_LEN];
} Reject;

typedef struct {
    Student *data;
    int size;
    int cap;
} StudentList;

typedef struct {
    Reject *data;
    int size;
    int cap;
} RejectList;

#endif

void sl_init(StudentList *l);
void sl_free(StudentList *l);
void sl_push(StudentList *l, const Student *s);
int  sl_copy(StudentList *dst, const StudentList *src);

void rl_init(RejectList *l);
void rl_free(RejectList *l);
void rl_push(RejectList *l, const Reject *r);

int  load_csv(const char *path, StudentList *all, RejectList *invalid, int *dataLines);
void filter_eligible(const StudentList *all, StudentList *eligible, RejectList *ineligible);
int write_valid_csv(const char *path, const StudentList *eligible);
int write_rejected_csv(const char *path, const RejectList *invalid, const RejectList *ineligible);
int write_ranked_csv(const char *path, const StudentList *ranked);

/* ---------- danh sach dong ---------- */
void sl_init(StudentList *l) {
    l->data = NULL;
    l->size = 0;
    l->cap = 0;
}

void sl_free(StudentList *l) {
    free(l->data);
    sl_init(l);
}

void sl_push(StudentList *l, const Student *s) {
    if (l->size == l->cap) {
        int nc = 16;
        if (l->cap > 0) {
            nc = l->cap * 2;
        }
        Student *p = realloc(l->data, (size_t)nc * sizeof(Student));
        if (!p) {
            fprintf(stderr, "Het bo nho\n");
            exit(1);
        }
        l->data = p;
        l->cap = nc;
    }
    l->data[l->size++] = *s;
}
int sl_copy(StudentList *dst, const StudentList *src) {
    sl_free(dst);
    if (src->size == 0) {
        return 0;
    }
    dst->data = malloc((size_t)src->size * sizeof(Student));
    if (!dst->data) {
        return -1;
    }
    memcpy(dst->data, src->data, (size_t)src->size * sizeof(Student));
    dst->size = dst->cap = src->size;
    return 0;
}

void rl_init(RejectList *l) {
    l->data = NULL;
    l->size = 0;
    l->cap = 0;
}

void rl_free(RejectList *l) {
    free(l->data);
    rl_init(l);
}

void rl_push(RejectList *l, const Reject *r) {
    if (l->size == l->cap) {
        int nc = 16;
        if (l->cap > 0) {
            nc = l->cap * 2;
        }
        Reject *p = realloc(l->data, (size_t)nc * sizeof(Reject));
        if (!p) {
            fprintf(stderr, "Het bo nho\n");
            exit(1);
        }
        l->data = p;
        l->cap = nc;
    }
    l->data[l->size++] = *r;
}

/* ---------- tien ich xu ly chuoi ---------- */
static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    char *e = s + strlen(s);
    while (e > s && isspace((unsigned char)e[-1])) {
        *--e = '\0';
    }
    return s;
}

static int ieq(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

/* Tach dong thanh cac truong theo dau phay; tra ve so truong (co the > max) */
static int split_csv(char *line, char *fields[], int max) {
    int n = 0;
    char *p = line;
    for (;;) {
        char *c = strchr(p, ',');
        if (n < max) {
            fields[n] = p;
        }
        n++;
        if (!c) {
            break;
        }
        *c = '\0';
        p = c + 1;
    }
    return n;
}

static int parse_double(const char *s, double *out) {
    char *end;
    errno = 0;
    if (*s == '\0') {
        return 0;
    }
    double v = strtod(s, &end);
    if (errno || *end != '\0') {
        return 0;
    }
    *out = v;
    return 1;
}

static int parse_int(const char *s, int *out) {
    char *end;
    errno = 0;
    if (*s == '\0') {
        return 0;
    }
    long v = strtol(s, &end, 10);
    if (errno || *end != '\0' || v < -2147483647L || v > 2147483647L) {
        return 0;
    }
    *out = (int)v;
    return 1;
}

static void add_invalid(RejectList *inv, int line, const char *id, const char *reason) {
    Reject r;
    memset(&r, 0, sizeof r);
    r.line = line;
    r.isInvalid = 1;
    if (id != NULL) {
        snprintf(r.id, ID_LEN, "%s", id);
    }
    snprintf(r.reason, REASON_LEN, "%s", reason);
    rl_push(inv, &r);
}

/* ---------- F1: doc va kiem tra CSV ---------- */
int load_csv(const char *path, StudentList *all, RejectList *invalid, int *dataLines) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return -1;
    }

    sl_free(all);
    rl_free(invalid);
    *dataLines = 0;

    char buf[1024];
    int lineNo = 0, seq = 0, firstNonEmpty = 1;

    while (fgets(buf, sizeof buf, f)) {
        lineNo++;
        /* dong qua dai: bo phan con lai de khong lech so dong */
        if (!strchr(buf, '\n') && !feof(f)) {
            int ch;
            while ((ch = fgetc(f)) != '\n' && ch != EOF) {
            }
            (*dataLines)++;
            add_invalid(invalid, lineNo, "", "dong qua dai (>1023 ky tu)");
            firstNonEmpty = 0;
            continue;
        }
        char copy[1024];
        snprintf(copy, sizeof copy, "%s", buf);
        char *line = trim(copy);
        if (*line == '\0') {
            continue;
        }
        /* bo BOM UTF-8 neu co */
        if ((unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF) line += 3;

        char *fld[8];
        char tmp[1024];
        snprintf(tmp, sizeof tmp, "%s", line);
        int n = split_csv(tmp, fld, 8);
        for (int i = 0; i < n && i < 8; i++) {
            fld[i] = trim(fld[i]);
        }

        /* dong tieu de */
        if (firstNonEmpty) {
            firstNonEmpty = 0;
            if (n >= 1 && (ieq(fld[0], "id") == 0
                    || ieq(fld[0], "ma") == 0
                    || ieq(fld[0], "masv") == 0)) {
                continue;
            }
        }
        (*dataLines)++;

        if (n != 5) {
            char msg[REASON_LEN];
            snprintf(msg, sizeof msg, "can 5 cot, dong co %d cot", n);
            const char *id = "";
            if (n >= 1) {
                id = fld[0];
            }
            add_invalid(invalid, lineNo, id, msg);
            continue;
        }

        Student s;
        memset(&s, 0, sizeof s);
        const char *err = NULL;
        if (fld[0][0] == '\0') {
            err = "ma sinh vien rong";
        } else if (strlen(fld[0]) >= ID_LEN) {
            err = "ma sinh vien qua dai";
        } else if (strlen(fld[1]) >= NAME_LEN) {
            err = "ho ten qua dai";
        } else if (!parse_double(fld[2], &s.gpa)) {
            err = "GPA khong phai so";
        } else if (s.gpa < 0.0 || s.gpa > 4.0) {
            err = "GPA ngoai khoang [0,4]";
        } else if (!parse_int(fld[3], &s.credits)) {
            err = "tin chi khong phai so nguyen";
        } else if (s.credits < 0) {
            err = "tin chi am";
        } else if (!parse_int(fld[4], &s.discipline)) {
            err = "ky luat khong phai so nguyen";
        } else if (s.discipline != 0 && s.discipline != 1) {
            err = "ky luat phai la 0 hoac 1";
        }

        if (!err) {
            for (int i = 0; i < all->size; i++) {
                if (strcmp(all->data[i].id, fld[0]) == 0) {
                    err = "ma sinh vien bi trung";
                    break;
                }
            }
        }
        if (err) {
            add_invalid(invalid, lineNo, fld[0], err);
            continue;
        }

        snprintf(s.id, ID_LEN, "%s", fld[0]);
        snprintf(s.name, NAME_LEN, "%s", fld[1]);
        s.inputIndex = ++seq;
        sl_push(all, &s);
    }
    fclose(f);
    return 0;
}

/* ---------- F2: loc dieu kien ---------- */
void filter_eligible(const StudentList *all, StudentList *eligible, RejectList *ineligible) {
    sl_free(eligible);
    rl_free(ineligible);
    for (int i = 0; i < all->size; i++) {
        const Student *s = &all->data[i];
        char why[REASON_LEN] = "";
        if (s->gpa < MIN_GPA - 1e-9)
            snprintf(why + strlen(why), sizeof why - strlen(why), "GPA %.2f < %.2f; ", s->gpa, MIN_GPA);
        if (s->credits < MIN_CREDITS)
            snprintf(why + strlen(why), sizeof why - strlen(why), "tin chi %d < %d; ", s->credits, MIN_CREDITS);
        if (s->discipline != 0)
            snprintf(why + strlen(why), sizeof why - strlen(why), "co vi pham ky luat; ");
        if (why[0] == '\0') {
            sl_push(eligible, s);
        } else {
            size_t L = strlen(why);
            if (L >= 2) {
                why[L - 2] = '\0';
            }
            Reject r;
            memset(&r, 0, sizeof r);
            r.line = 0;
            r.isInvalid = 0;
            snprintf(r.id, ID_LEN, "%s", s->id);
            snprintf(r.reason, REASON_LEN, "%s", why);
            rl_push(ineligible, &r);
        }
    }
}

/* ---------- F6: ghi file ---------- */
int write_valid_csv(const char *path, const StudentList *e) {
    FILE *f = fopen(path, "w");
    if (!f) {
        return -1;
    }
    fprintf(f, "id,name,gpa,credits,discipline\n");
    for (int i = 0; i < e->size; i++) {
        fprintf(f, "%s,%s,%.2f,%d,%d\n", e->data[i].id, e->data[i].name,
                e->data[i].gpa, e->data[i].credits, e->data[i].discipline);
    }
    fclose(f);
    return 0;
}

int write_rejected_csv(const char *path, const RejectList *inv, const RejectList *inel) {
    FILE *f = fopen(path, "w");
    if (!f) {
        return -1;
    }
    fprintf(f, "type,line,id,reason\n");
    for (int i = 0; i < inv->size; i++) {
        fprintf(f, "INVALID,%d,%s,%s\n", inv->data[i].line, inv->data[i].id, inv->data[i].reason);
    }
    for (int i = 0; i < inel->size; i++) {
        fprintf(f, "INELIGIBLE,,%s,%s\n", inel->data[i].id, inel->data[i].reason);
    }
    fclose(f);
    return 0;
}

int write_ranked_csv(const char *path, const StudentList *r) {
    FILE *f = fopen(path, "w");
    if (!f) {
        return -1;
    }
    fprintf(f, "rank,id,name,gpa,credits,discipline\n");
    for (int i = 0; i < r->size; i++) {
        fprintf(f, "%d,%s,%s,%.2f,%d,%d\n", i + 1, r->data[i].id, r->data[i].name,
                r->data[i].gpa, r->data[i].credits, r->data[i].discipline);
    }
    fclose(f);
    return 0;
}
