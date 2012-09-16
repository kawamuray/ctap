#ifndef _CTAP_H_
#define _CTAP_H_

#include <stdio.h>   /* fdopen(3)              */
#include <stdlib.h>  /* exit(3)                */
#include <string.h>  /* strlen(3)              */
#include <stdarg.h>  /* va_start(3), va_end(3) */
#include <float.h>   /* DBL_EPSILON            */
#include <math.h>    /* fabs(3)                */
#include <assert.h>  /* assert(3)              */

#ifndef SUBTEST_MAX_DEPTH
#define SUBTEST_MAX_DEPTH 10
#endif

#define INDENT_LEVEL      4

typedef unsigned int uint;

enum bool_mode { COND_TRUE, COND_FALSE };

static struct {
    int  plan;
    uint run;
    uint pass;
    uint fail;
} tests[SUBTEST_MAX_DEPTH];

static uint current;

#define TESTS_PLAN tests[current].plan
#define TESTS_RUN  tests[current].run
#define TESTS_PASS tests[current].pass
#define TESTS_FAIL tests[current].fail

static FILE *tapout;
static FILE *msgout;

#define FL __FILE__, __LINE__

/* Used to handy output 'got - expected' pair in _is_* functions */
#define GOT(got, fmt, ...)      diag("    %s: " fmt "\n", got, ##__VA_ARGS__)
#define EXP(expected, fmt, ...) diag("    %s: " fmt "\n", (bmode == COND_FALSE) ? "anything else" : expected, ##__VA_ARGS__)


/**
 * Print a whitespace to make indent.
 */
static inline void pindent(FILE *out)
{
    fprintf(out, "%*s", INDENT_LEVEL*current, "");
}

/**
 * Bail out from the test.
 *
 *     bail(why, ...);
 *
 * @param why a reason of aborting test.
 */
void bail(char *why, ...)
{
    va_list ap;
    va_start(ap, why);
    vfprintf(msgout, why, ap);
    va_end(ap);
    exit(255);
}

/**
 * Print a diagnostic message to message output.
 *
 *     diag(msg, ...);
 *
 * @param msg a format string of diagnostics
*/
void diag(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    vfprintf(msgout, msg, ap);
    va_end(ap);
}

/**
 * Initialize test with informing number of tests that you are planning going to run.
 * WARNING(to perl users): This function is internally initialize some environments. So you cannot omit
 *                         to call this function like you are usually doing with Test::* the modules of perl.
 *
 *     plan(number_of_tests);
 *     plan(-1);              // You don't know how many tests will run.
 *
 * @see done_testing()
 * @param number_of_tests a number of tests that you are planning going to run.
 */
void plan(int ntests)
{
    /* Initialize output stream for TAP output and messages output */
    if (tapout == NULL || msgout == NULL) {
        /* TAP messages are should be writen to stdout */
        if ((tapout = fdopen(fileno(stdout), "w")) == NULL)
            bail("Failed to fdopen stdout: ");
        if (setvbuf(tapout, NULL, _IONBF, 0) != 0)
            bail("Failed to setvbuf to TAP output: ");
        /* Comments are should be writen to stderr */
        if ((msgout = fdopen(fileno(stderr), "w")) == NULL)
            bail("Failed to fdopen stderr: ");
        if (setvbuf(msgout, NULL, _IONBF, 0) != 0)
            bail("Failed to setvbuf to Message output: ");
    }

    TESTS_PLAN = ntests;
    TESTS_RUN = TESTS_PASS = TESTS_FAIL = 0;
}

/**
 * If you were passed -1 for plan(), you must call this function at end of the test.
 *
 *     done_testing(number_of_tests);
 *     done_testing(-1);              // Use the number of counter as the number that you are expected.
 *
 * @see plan()
 * @param number_of_tests a number of tests that you are planning going to run.
 */
void done_testing(int ntests)
{
    if (ntests < 0) {
        TESTS_PLAN = TESTS_RUN;
    }

    pindent(tapout);
    fprintf(tapout, "1..%d\n", TESTS_RUN);
    if (TESTS_RUN != TESTS_PLAN) {
        pindent(msgout);
        fprintf(msgout, "# Looks like you planned %u tests but run %u\n", TESTS_PLAN, TESTS_RUN);
    }
    if (TESTS_FAIL) {
        pindent(msgout);
        fprintf(msgout, "# Looks like you failed %u test of %u\n", TESTS_FAIL, TESTS_RUN);
    }
}


/**
 * A very core function of testing framework. This function is internally use only.
 * Check the result of test, increment the test counters.
 * Then output TAP messages and messages for users.
 * Call done_testing() if the number of tests ran is equals to the number of tests planned
 *
 * @see done_testing()
 * @param test  a result of test. 0 for fail, other for pass.
 * @param bmode a boolean mode for test result. If it is set to COND_FALSE, test result will reversed on evaluation.
 * @param file  a filename of this test has been ran.
 * @param line  a line number of the file where this test has been ran.
 * @param name  a short description of test.
 * @param ap    a argument that should pass to vfprintf(3) with name
 */
int __ok(uint test, enum bool_mode bmode,
         const char *file, uint line, const char *name, va_list ap)
{
    va_list ap_copy;
    uint namelen = strlen(name);
    va_copy(ap_copy, ap);

    TESTS_RUN++;
    pindent(tapout);

    if (bmode == COND_FALSE) test = !test;

    if (!test) fputs("not ", tapout);

    fprintf(tapout, "ok %d", TESTS_RUN);

    /* Print tests name if specified */
    if (namelen) {
        fputs(" - ", tapout);
        vfprintf(tapout, name, ap);
    }
    fputc('\n', tapout);

    if (test) {
        TESTS_PASS++;
    } else {
        TESTS_FAIL++;

        /* Expecing output example:
         * #   Failed test at test.c line 10.
         * #   Failed test "MyTest" at test.c line 10.
         */

        pindent(msgout);
        fputs("#   Failed test", msgout);
        if (namelen) {
            fputs(" \"", msgout);
            vfprintf(msgout, name, ap_copy);
            fputc('"', msgout);
        }
        fprintf(msgout, " at %s line %u\n", file, line);
    }

    if (TESTS_RUN == TESTS_PLAN)
        done_testing(TESTS_PLAN);

    return test;
}

/**
 * Inform a test has been passed.
 *
 *     pass();
 *     pass(testname, ...);
 *
 * @param testname a short description of test.
 */
#define pass(...) _pass(FL, ""__VA_ARGS__)

int _pass(const char *file, uint line, const char *name, ...)
{
    va_list ap;
    va_start(ap, name);
    __ok(1, COND_TRUE, file, line, name, ap);
    va_end(ap);

    return 1;
}    

/**
 * Inform a test has been failed.
 *
 *     fail();
 *     fail(testname, ...);
 *
 * @param testname a short description of test.
 */
#define fail(...) _fail(FL, ""__VA_ARGS__)

int _fail(const char *file, uint line, const char *name, ...)
{
    va_list ap;
    va_start(ap, name);
    __ok(0, COND_TRUE, file, line, name, ap);
    va_end(ap);

    return 0;
}    

/**
 * Inform a result of some test.
 *
 *     ok(got == expected);
 *     ok(cmp_func(got, expected);
 *     ok(test_result, name, ...);
 *
 * @param test a result of test, 0 for fail, other for pass.
 * @param name a short description of test.
 */
#define ok(test, ...) _ok(test, COND_TRUE, FL, ""__VA_ARGS__)

int _ok(uint test, enum bool_mode bmode,
        const char *file, uint line, const char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    result = __ok(test, bmode, file, line, name, ap);

    va_end(ap);
    return result;
}

/**
 * Handy test function to compare that integer values are same.
 *
 *     is_int(got, expected);
 *     is_int(got, expected, name, ...);
 *
 * @param got      a integer value that you've got.
 * @param expected a integer value that you've expected.
 * @param name     a short description of test.
 */
#define   is_int(got, expected, ...) _is_int(got, expected, COND_TRUE , FL, ""__VA_ARGS__)

/**
 * Handy test function to compare that integer values are not same.
 *
 *     isnt_int(got, expected);
 *     isnt_int(got, expected, name, ...);
 *
 * @param got      a integer value that you've got.
 * @param expected a integer value that you've expected.
 * @param name     a short description of test.
 */
#define isnt_int(got, expected, ...) _is_int(got, expected, COND_FALSE, FL, ""__VA_ARGS__)

int _is_int(long got, long expected, enum bool_mode bmode,
            const char *file, uint line, const char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((got == expected), bmode, file, line, name, ap))) {
        GOT("     got", "%ld", got);
        EXP("expected", "%ld", expected);
    }

    va_end(ap);
    return result;
}

/**
 * Handy test function to compare that float values are same.
 *
 *     is_double(got, expected);
 *     is_double(got, expected, name, ...);
 *
 * @param got      a float value that you've got.
 * @param expected a float value that you've expected.
 * @param name     a short description of test.
 */
#define   is_double(got, expected, ...) _is_double(got, expected, COND_TRUE , FL, ""__VA_ARGS__)

/**
 * Handy test function to compare that float values are not same.
 *
 *     isnt_double(got, expected);
 *     isnt_double(got, expected, name, ...);
 *
 * @param got      a float value that you've got.
 * @param expected a float value that you've expected.
 * @param name     a short description of test.
 */
#define isnt_double(got, expected, ...) _is_double(got, expected, COND_FALSE, FL, ""__VA_ARGS__)

int _is_double(double got, double expected, enum bool_mode bmode,
               const char *file, uint line, const char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((fabs(got - expected) < DBL_EPSILON), bmode, file, line, name, ap))) {
        GOT("     got", "%g", got);
        EXP("expected", "%g", expected);
    }

    va_end(ap);
    return result;
}

/**
 * Handy test function to compare that character values are same.
 *
 *     is_char(got, expected);
 *     is_char(got, expected, name, ...);
 *
 * @param got      a character value that you've got.
 * @param expected a character value that you've expected.
 * @param name     a short description of test.
 */
#define   is_char(got, expected, ...) _is_char(got, expected, COND_TRUE , FL, ""__VA_ARGS__)

/**
 * Handy test function to compare that character values are not same.
 *
 *     is_char(got, expected);
 *     is_char(got, expected, name, ...);
 *
 * @param got      a character value that you've got.
 * @param expected a character value that you've expected.
 * @param name     a short description of test.
 */
#define isnt_char(got, expected, ...) _is_char(got, expected, COND_FALSE, FL, ""__VA_ARGS__)

int _is_char(char got, char expected, enum bool_mode bmode,
             const char *file, uint line, const char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((got == expected), bmode, file, line, name, ap))) {
        GOT("     got", "%c", got);
        EXP("expected", "%c", expected);
    }

    va_end(ap);
    return result;
}

/**
 * Handy test function to compare that string values are same.
 * This function use strcmp(3) to compare strings, so you must
 * terminate strings to pass this function by '\0'.
 *
 *     is_str(got, expected);
 *     is_str(got, expected, name, ...);
 *
 * @param got      a string value that you've got. Must be terminated with '\0'.
 * @param expected a string value that you've expected. Must be terminated with '\0'.
 * @param name     a short description of test.
 */
#define   is_str(got, expected, ...) _is_str(got, expected, COND_TRUE , FL, ""__VA_ARGS__)

/**
 * Handy test function to compare that string values are not same.
 * This function use strcmp(3) to compare strings, so you must
 * terminate strings to pass this function by '\0'.
 *
 *     isnt_str(got, expected);
 *     isnt_str(got, expected, name, ...);
 *
 * @param got      a string value that you've got. Must be terminated with '\0'.
 * @param expected a string value that you've expected. Must be terminated with '\0'.
 * @param name     a short description of test.
 */
#define isnt_str(got, expected, ...) _is_str(got, expected, COND_FALSE, FL, ""__VA_ARGS__)

int _is_str(const char *got, const char *expected, enum bool_mode bmode,
            const char *file, uint line, const char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok(!strcmp(got, expected), bmode, file, line, name, ap))) {
        GOT("     got", "%s", got);
        EXP("expected", "%s", expected);
    }

    va_end(ap);
    return result;
}

/**
 * Handy test function to compare that pointer values are same.
 *
 *     is_p(got, expected);
 *     is_p(got, expected, name, ...);
 *
 * @param got      a pointer value that you've got.
 * @param expected a pointer value that you've expected.
 * @param name     a short description of test.
 */
#define   is_p(got, expected, ...) _is_p(got, expected, COND_TRUE , FL, ""__VA_ARGS__)

/**
 * Handy test function to compare that pointer values are not same.
 *
 *     is_p(got, expected);
 *     is_p(got, expected, name, ...);
 *
 * @param got      a pointer value that you've got.
 * @param expected a pointer value that you've expected.
 * @param name     a short description of test.
 */
#define isnt_p(got, expected, ...) _is_p(got, expected, COND_FALSE, FL, ""__VA_ARGS__)

int _is_p(const void *got, const void *expected, enum bool_mode bmode,
          const char *file, uint line, const char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((got == expected), bmode, file, line, name, ap))) {
        GOT("     got", "%p", got);
        EXP("expected", "%p", expected);
    }

    va_end(ap);
    return result;
}

/**
 * Handy test function to compare that two memory spaces are same.
 *
 *     is_p(got, expected, size);
 *     is_p(got, expected, size, name, ...);
 *
 * @param got      a address that pointing memory that you've got.
 * @param expected a address that pointing memory that you've expected.
 * @param size     a memory size that you would like to compare.
 * @param name     a short description of test.
 */
#define   is_mem(got, expected, size, ...) _is_mem(got, expected, size, COND_TRUE , FL, ""__VA_ARGS__)

/**
 * Handy test function to compare that two memory spaces are not same.
 *
 *     is_p(got, expected, size);
 *     is_p(got, expected, size, name, ...);
 *
 * @param got      a address that pointing memory that you've got.
 * @param expected a address that pointing memory that you've expected.
 * @param size     a memory size that you would like to compare.
 * @param name     a short description of test.
 */
#define isnt_mem(got, expected, size, ...) _is_mem(got, expected, size, COND_FALSE, FL, ""__VA_ARGS__)

int _is_mem(const void *got, const void *expected, size_t size, enum bool_mode bmode,
            const char *file, uint line, const char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok(!memcmp(got, expected, size), bmode, file, line, name, ap))) {
        GOT("     got", "%p(+0x%x)", got, size);
        EXP("expected", "%p(+0x%x)", expected, size);
    }

    va_end(ap);
    return result;
}

/**
 * Run the given function as its own little test with its own plan and its own result.
 * The main test counts this as a single test using the result of the whole subtest.
 *
 *     void subtest_foobar(void)
 *     {
 *         is_int(1, 1, "One is totally one");
 *     }
 *
 *     int main(void)
 *     {
 *         plan(-1);
 *         subtest("Subtest for testing subtest", subtest_foobar);
 *         done_testing(-1);
 *     }
 *
 * @param name a short description of subtest.
 * @param func a pointer to function which should run as a subtest.
 */
#define subtest(name, func) _subtest(name, func, FL)

int _subtest(const char *name, void (*func)(void),
              const char *file, uint line)
{
    uint subtestp;

    // Push tests status stack
    if (++current == SUBTEST_MAX_DEPTH) {
        bail("Too deep subtest nesting. You can change macro SUBTEST_MAX_DEPTH to change this value.");
    }

    plan(-1);
    func();
    
    done_testing(-1);

    // Pop tests status stack
    subtestp = current--;

    assert(current >= 0);

    uint sub_result;
    if (tests[subtestp].run == 0) {
        sub_result = _fail(file, line, "No tests run for subtest \"%s\"", name);
    } else {
        sub_result = __ok((tests[subtestp].run == tests[subtestp].pass), 0, file, line, name, NULL);
    }
}

#endif /* _CTAP_H_ */
