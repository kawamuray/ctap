#ifndef _CTAP_H_
#define _CTAP_H_

#include <stddef.h>

typedef unsigned int uint;

#define ok(test, ...) _ok(test, 0, __FILE__, __LINE__, ""__VA_ARGS__)

#define   is_int(got, expected, ...) _is_int(got, expected, 0, __FILE__, __LINE__, ""__VA_ARGS__)
#define isnt_int(got, expected, ...) _is_int(got, expected, 1, __FILE__, __LINE__, ""__VA_ARGS__)

#define   is_double(got, expected, ...) _is_double (got, expected, 0, __FILE__, __LINE__, ""__VA_ARGS__)
#define isnt_double(got, expected, ...) _is_double (got, expected, 1, __FILE__, __LINE__, ""__VA_ARGS__)

#define   is_char(got, expected, ...) _is_char(got, expected, 0, __FILE__, __LINE__, ""__VA_ARGS__)
#define isnt_char(got, expected, ...) _is_char(got, expected, 1, __FILE__, __LINE__, ""__VA_ARGS__)

#define   is_str(got, expected, ...) _is_str(got, expected, 0, __FILE__, __LINE__, ""__VA_ARGS__)
#define isnt_str(got, expected, ...) _is_str(got, expected, 1, __FILE__, __LINE__, ""__VA_ARGS__)

#define   is_p(got, expected, ...) _is_p(got, expected, 0, __FILE__, __LINE__, ""__VA_ARGS__)
#define isnt_p(got, expected, ...) _is_p(got, expected, 1, __FILE__, __LINE__, ""__VA_ARGS__)

#define   is_mem(got, expected, size, ...) _is_mem(got, expected, size, 0, __FILE__, __LINE__, ""__VA_ARGS__)
#define isnt_mem(got, expected, size, ...) _is_mem(got, expected, size, 1, __FILE__, __LINE__, ""__VA_ARGS__)

#define PINDENT(out) do { int i = 0; for (i = 0; i < INDENT_LEVEL*current; i++) fputc(' ', (out)); } while (0)

#define diag(msg, ...) do { PINDENT(msgout); fputs("# ", msgout); fprintf(msgout, msg, ##__VA_ARGS__); } while (0)

/* Used to handy output 'got - expected' pair in _is_* functions */
#define GOT(got, fmt, ...)      diag("    %s: " fmt "\n", got, ##__VA_ARGS__)
#define EXP(expected, fmt, ...) diag("    %s: " fmt "\n", (not) ? "anything else" : expected, ##__VA_ARGS__)

#define pass(...) _pass(""__VA_ARGS__, __FILE__, __LINE__)
#define fail(...) _fail(""__VA_ARGS__, __FILE__, __LINE__)

#define subtest(name, func) _subtest(name, func, __FILE__, __LINE__)

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
