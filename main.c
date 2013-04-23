#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>
#include "minunit.h"
#include "skiplist.h"
#include "keylist.h"

int tests_passed = 0;
int tests_failed = 0;
sl *slist = NULL;
kl *klist = NULL;
int vals[] = {11,12,13,14,15,16,17};
int keys[] = {9, 7, 12, 0, 7455, -1, 15};

#define ARRAY_LEN(arr) (sizeof(arr)/sizeof(0[arr]))

static char *sl_list_length(void) {
    int len = sl_len(slist);

    mu_assert("list length is incorrect", len == ARRAY_LEN(vals));
    return 0;
}

static char *sl_test_find(void) {
    int *v;
    for(int i = 0; i < ARRAY_LEN(keys); i++) {
        v = sl_find(slist, keys[i]);
        mu_assert("inserted item not found", v != NULL);
        mu_assert("value of found item incorrect", *v == vals[i]);
    }

    v = sl_find(slist, 99);
    mu_assert("find key not in list didn't return NULL", v == NULL);

    return 0;
}

static char *sl_test_update(void) {
    int v1 = 5;
    int v2 = 6;
    int *v;
    sl *slist = sl_create();

    sl_insert(slist, 9, &v1);
    sl_insert(slist, 9, &v2);

    v = sl_find(slist, 9);
    mu_assert("updating list with one element fails", *v == v2);
    sl_destroy(slist);
    return 0;
}

static char *sl_test_removal(void) {
    int *v;
    int r;
    int len;

    len = sl_len(slist);
    r = sl_remove(slist, keys[0]);
    v = sl_find(slist, keys[0]);

    mu_assert("find key removed from list didn't return NULL", v == NULL);
    mu_assert("wrong length after key removed", len-1 == sl_len(slist));

    r = sl_remove(slist, -1);
    mu_assert("can't remove first key", r == 1);
    r = sl_remove(slist, 7455);
    mu_assert("can't remove last key", r == 1);
    mu_assert("wrong length after keys removed", len-3 == sl_len(slist));

    return 0;
}

static char *kl_list_length(void) {
    int len = kl_len(klist);

    mu_assert("list length is incorrect", len == ARRAY_LEN(vals));
    return 0;
}

static char *kl_test_find(void) {
    int *v;
    for(int i = 0; i < ARRAY_LEN(keys); i++) {
        v = kl_find(klist, keys[i]);
        mu_assert("inserted item not found", v != NULL);
        mu_assert("value of found item incorrect", *v == vals[i]);
    }

    v = kl_find(klist, 99);
    mu_assert("find key not in list didn't return NULL", v == NULL);

    return 0;
}

static char *kl_test_update(void) {
    int v1 = 5;
    int v2 = 6;
    int *v;
    kl *klist = kl_create();

    kl_insert(klist, 9, &v1);
    kl_insert(klist, 9, &v2);

    v = kl_find(klist, 9);
    mu_assert("updating list with one element fails", *v == v2);
    kl_destroy(klist);
    return 0;
}

static char *kl_test_removal(void) {
    int *v;
    int r;
    int len;

    len = kl_len(klist);
    r = kl_remove(klist, keys[0]);
    v = kl_find(klist, keys[0]);

    mu_assert("find key removed from list didn't return NULL", v == NULL);
    mu_assert("wrong length after key removed", len-1 == kl_len(klist));

    r = kl_remove(klist, -1);
    mu_assert("can remove first key", r == 1);
    r = kl_remove(klist, 7455);
    mu_assert("can remove last key", r == 1);
    mu_assert("wrong length after keys removed", len-3 == kl_len(klist));

    return 0;
}

unsigned long ftime_diff(struct _timeb *a, struct _timeb *b) {
    return (b->time - a->time)*1000 + (b->millitm - a->millitm);
}

static char *test_speed_compare(void) {
    struct _timeb start;
    struct _timeb finish;
    unsigned long sl_time_taken;
    unsigned long kl_time_taken;
    int keys[10000];
    int vals[10000];
    int test_keys[100];
    sl *slist = sl_create();
    kl *klist = kl_create();
    int *v;

    _ftime(&start);
    /* generate long list (same for both) */
    for(int i = 0; i < 10000; i++) {
        keys[i] = rand();
        vals[i] = rand();
    }
    for(int i = 0; i < 100; i++) {
        test_keys[i] = rand() % 10000;
    }

    /* fill lists with it (time this) */
    _ftime(&start);
    for(int i = 0; i < 10000; i++) {
        kl_insert(klist, keys[i], &vals[i]);
    }
    _ftime(&finish);
    printf("kl: inserted 10,000 keys in %lums\n", ftime_diff(&start, &finish));

    _ftime(&start);
    for(int i = 0; i < 10000; i++) {
        sl_insert(slist, keys[i], &vals[i]);
    }
    _ftime(&finish);
    printf("sl: inserted 10,000 keys in %lums\n", ftime_diff(&start, &finish));

    /* find random values in it (same for both) n times, average time taken */
    _ftime(&start);
    for(int i = 0; i < 1000; i++) {
        for(int j = 0; j < 100; j++) {
            v = sl_find(slist, keys[test_keys[j]]);
        }
    }
    _ftime(&finish);
    sl_time_taken = ftime_diff(&start, &finish);

    _ftime(&start);
    for(int i = 0; i < 1000; i++) {
        for(int j = 0; j < 100; j++) {
            v = kl_find(klist, keys[test_keys[j]]);
        }
    }
    _ftime(&finish);
    kl_time_taken = ftime_diff(&start, &finish);

    printf("sl: found 100,000 keys in %fs, %.2fkeys/s\n", sl_time_taken/1000.0, 100.0*100000/sl_time_taken);
    printf("kl: found 100,000 keys in %fs, %.2fkeys/s\n\n", kl_time_taken/1000.0, 100.0*100000/kl_time_taken);

    mu_assert("average sl find speed is slower than kl", sl_time_taken < kl_time_taken);

    sl_destroy(slist);
    kl_destroy(klist);

    return 0;
}

void test_setup(void) {
    srand(time(NULL));

    slist = sl_create();
    klist = kl_create();
    
    for(int i = 0; i < ARRAY_LEN(keys); i++) {
        sl_insert(slist, keys[i], &vals[i]);
    }
    sl_output(slist);

    printf("\n");

    for(int i = 0; i < ARRAY_LEN(keys); i++) {
        kl_insert(klist, keys[i], &vals[i]);
    }
    kl_output(klist);

    printf("\n");
}

void test_teardown(void) {
    sl_destroy(slist);
    kl_destroy(klist);
}

void run_all_tests(void) {
    test_setup();

    mu_run_test(sl_list_length, "sl: list length");
    mu_run_test(sl_test_find, "sl: find in list");
    mu_run_test(sl_test_update, "sl: update existing key");
    mu_run_test(sl_test_removal, "sl: remove from list");

    printf("\n");

    mu_run_test(kl_list_length, "kl: list length");
    mu_run_test(kl_test_find, "kl: find in list");
    mu_run_test(kl_test_update, "kl: update existing key");
    mu_run_test(kl_test_removal, "kl: remove from list");

    printf("\n");

    mu_run_test(test_speed_compare, "find in a long list is faster with SL");

    test_teardown();
}

int main(void) {
    run_all_tests();
    printf("Tests run: %d | Tests passed: %d | Tests failed: %d\n", \
        tests_failed + tests_passed, tests_passed, tests_failed);

    return 0;
}
