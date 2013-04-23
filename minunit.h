/* adjusted version of http://www.jera.com/techinfo/jtns/jtn002.html */
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test, name) do { char *message = test(); if (message) { printf("FAIL: %s (%s)\n", name, message); tests_failed++; } else { printf("pass: %s\n", name); tests_passed++; }} while (0)
extern int tests_passed;
extern int tests_failed;
