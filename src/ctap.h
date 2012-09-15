#ifndef _CTAP_H_
#define _CTAP_H_

#include <stddef.h>

typedef unsigned int uint;

#define COND_TRUE  0
#define COND_FALSE 1

#define FL __FILE__, __LINE__

/* Testing APIs */
#define          ok(test,                ...)  _ok        (test, COND_TRUE, FL, ""__VA_ARGS__)

#define      is_int(got, expected,       ...)  _is_int    (got, expected,       COND_TRUE , FL, ""__VA_ARGS__)
#define    isnt_int(got, expected,       ...)  _is_int    (got, expected,       COND_FALSE, FL, ""__VA_ARGS__)

#define   is_double(got, expected,       ...)  _is_double (got, expected,       COND_TRUE , FL, ""__VA_ARGS__)
#define isnt_double(got, expected,       ...)  _is_double (got, expected,       COND_FALSE, FL, ""__VA_ARGS__)

#define     is_char(got, expected,       ...)  _is_char   (got, expected,       COND_TRUE , FL, ""__VA_ARGS__)
#define   isnt_char(got, expected,       ...)  _is_char   (got, expected,       COND_FALSE, FL, ""__VA_ARGS__)

#define      is_str(got, expected,       ...)  _is_str    (got, expected,       COND_TRUE , FL, ""__VA_ARGS__)
#define    isnt_str(got, expected,       ...)  _is_str    (got, expected,       COND_FALSE, FL, ""__VA_ARGS__)

#define        is_p(got, expected,       ...)  _is_p      (got, expected,       COND_TRUE , FL, ""__VA_ARGS__)
#define      isnt_p(got, expected,       ...)  _is_p      (got, expected,       COND_FALSE, FL, ""__VA_ARGS__)

#define      is_mem(got, expected, size, ...)  _is_mem    (got, expected, size, COND_TRUE , FL, ""__VA_ARGS__)
#define    isnt_mem(got, expected, size, ...)  _is_mem    (got, expected, size, COND_FALSE, FL, ""__VA_ARGS__)

#define PINDENT(out) do { int i = 0; for (i = 0; i < INDENT_LEVEL*current; i++) fputc(' ', (out)); } while (0)

#define diag(msg, ...) do { PINDENT(msgout); fputs("# ", msgout); fprintf(msgout, msg, ##__VA_ARGS__); } while (0)

/* Used to handy output 'got - expected' pair in _is_* functions */
#define GOT(got, fmt, ...)      diag("    %s: " fmt "\n", got, ##__VA_ARGS__)
#define EXP(expected, fmt, ...) diag("    %s: " fmt "\n", (not) ? "anything else" : expected, ##__VA_ARGS__)

#define pass(...) _pass(FL, ""__VA_ARGS__)
#define fail(...) _fail(FL, ""__VA_ARGS__)

#define subtest(name, func) _subtest(name, func, FL)

extern void plan(int ntests);
extern void done_testing(int ntests);
extern int  _ok(uint test, uint not, const char *file, uint line, char *name, ...);
extern void bail(char *why, ...);


extern int _is_int   (long   got, long   expected, uint not, const char *file, uint line, char *name, ...);
extern int _is_double(double got, double expected, uint not, const char *file, uint line, char *name, ...);
extern int _is_char  (char   got, char   expected, uint not, const char *file, uint line, char *name, ...);
extern int _is_str   (char  *got, char  *expected, uint not, const char *file, uint line, char *name, ...);
extern int _is_p     (void  *got, void  *expected, uint not, const char *file, uint line, char *name, ...);

extern int _is_mem   (void *got, void *expected, uint not, size_t size, const char *file, uint line, char *name, ...);

#endif /* _CTAP_H_ */
