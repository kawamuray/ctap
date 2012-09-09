#include <stdio.h>   /* fdopen(3)              */
#include <stdlib.h>  /* exit(3)                */
#include <string.h>  /* strlen(3)              */
#include <stdarg.h>  /* va_start(3), va_end(3) */
#include <float.h>   /* DBL_EPSILON            */
#include <math.h>    /* fabs(3)                */
#include <assert.h>  /* assert(3)              */
#include "ctap.h"

#define SUBTEST_MAX_DEPTH 10
#define INDENT_LEVEL      4

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

void bail(char *why, ...)
{
    va_list ap;
    va_start(ap, why);
    vfprintf(msgout, why, ap);
    va_end(ap);
    exit(1);
}

void _pass(const char *file, uint line, char *name, ...)
{
    va_list ap;
    va_start(ap, name);
    __ok(1, 0, file, line, name, ap);
    va_end(ap);
}    

void _fail(const char *file, uint line, char *name, ...)
{
    va_list ap;
    va_start(ap, name);
    __ok(0, 0, file, line, name, ap);
    va_end(ap);
}    

void plan(int ntests)
{
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

void done_testing(int ntests)
{
    if (ntests < 0) {
        TESTS_PLAN = TESTS_RUN;
    }

    PINDENT(tapout);
    fprintf(tapout, "1..%d\n", TESTS_RUN);
    if (TESTS_RUN != TESTS_PLAN) {
        PINDENT(msgout);
        fprintf(msgout, "# Looks like you planned %u tests but run %u\n", TESTS_PLAN, TESTS_RUN);
    }
    if (TESTS_FAIL) {
        PINDENT(msgout);
        fprintf(msgout, "# Looks like you failed %u test of %u\n", TESTS_FAIL, TESTS_RUN);
    }
}

void _subtest(char *name, void *(*func)(void), const char *file, uint line)
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

    if (tests[subtestp].run == 0) {
        _fail(file, line, "No tests run for subtest \"%s\"", name);
    } else {
        __ok((tests[subtestp].run == tests[subtestp].pass), 0, file, line, name, NULL);
    }
}

int __ok(uint test, uint not, const char *file, uint line, char *name, va_list ap)
{
    va_list ap_copy;
    uint namelen = strlen(name);
    va_copy(ap_copy, ap);

    TESTS_RUN++;
    PINDENT(tapout);

    if (not) test = !test;

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

        PINDENT(msgout);
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

int _ok(uint test, uint not, const char *file, uint line, char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    result = __ok(test, not, file, line, name, ap);

    va_end(ap);
    return result;
}

int _is_int(long got, long expected, uint not, const char *file, uint line, char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((got == expected), not, file, line, name, ap))) {
        GOT("     got", "%ld", got);
        EXP("expected", "%ld", expected);
    }

    va_end(ap);
    return result;
}

int _is_double(double got, double expected, uint not, const char *file, uint line, char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((fabs(got - expected) < DBL_EPSILON), not, file, line, name, ap))) {
        GOT("     got", "%g", got);
        EXP("expected", "%g", expected);
    }

    va_end(ap);
    return result;
}

int _is_char(char got, char expected, uint not, const char *file, uint line, char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((got == expected), not, file, line, name, ap))) {
        GOT("     got", "%c", got);
        EXP("expected", "%c", expected);
    }

    va_end(ap);
    return result;
}

int _is_str(char *got, char *expected, uint not, const char *file, uint line, char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok(!strcmp(got, expected), not, file, line, name, ap))) {
        GOT("     got", "%s", got);
        EXP("expected", "%s", expected);
    }

    va_end(ap);
    return result;
}

int _is_p(void *got, void *expected, uint not, const char *file, uint line, char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok((got == expected), not, file, line, name, ap))) {
        GOT("     got", "%p", got);
        EXP("expected", "%p", expected);
    }

    va_end(ap);
    return result;
}

int _is_mem(void *got, void *expected, size_t size, uint not, const char *file, uint line, char *name, ...)
{
    va_list ap;
    uint result;
    va_start(ap, name);

    if (!(result = __ok(!memcmp(got, expected, size), not, file, line, name, ap))) {
        GOT("     got", "%p(+0x%x)", got, size);
        EXP("expected", "%p(+0x%x)", expected, size);
    }

    va_end(ap);
    return result;
}
