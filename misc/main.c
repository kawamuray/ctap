#include "newctap.h"

int main(void)
{
    int testbuf[10] = { 100 };
    int invalidbuf[10] = { 200 };

    plan(-1);

    ok(1);
    ok(1, "ok with comment(%d)", 1);
    ok(0);
    ok(0, "ok with comment(%d)", 0);

    is_int(1, 1);
    is_int(1, 1, "is_int with comment(%d <=> %d)", 1, 1);
    is_int(1, 0);
    is_int(1, 0, "is_int with comment(%d <=> %d)", 1, 0);
    isnt_int(1, 1);
    isnt_int(1, 1, "isnt_int with comment(%d <=> %d)", 1, 1);
    isnt_int(1, 0);
    isnt_int(1, 0, "isnt_int with comment(%d <=> %d)", 1, 0);

    is_double(1.0, 1.0);
    is_double(1.0, 1.0, "is_double with comment(%lf <=> %lf)", 1.0, 1.0);
    is_double(1.0, 0.0);
    is_double(1.0, 0.0, "is_double with comment(%lf <=> %lf)", 1.0, 0.0);
    isnt_double(1.0, 1.0);
    isnt_double(1.0, 1.0, "isnt_double with comment(%lf <=> %lf)", 1.0, 1.0);
    isnt_double(1.0, 0.0);
    isnt_double(1.0, 0.0, "isnt_double with comment(%lf <=> %lf)", 1.0, 0.0);

    is_char('a', 'a');
    is_char('a', 'a', "is_char with comment(%c <=> %c)", 'a', 'a');
    is_char('a', 'b');
    is_char('a', 'b', "is_char with comment(%c <=> %c)", 'a', 'b');
    isnt_char('a', 'a');
    isnt_char('a', 'a', "isnt_char with comment(%c <=> %c)", 'a', 'a');
    isnt_char('a', 'b');
    isnt_char('a', 'b', "isnt_char with comment(%c <=> %c)", 'a', 'b');

    is_str("foo", "foo");
    is_str("foo", "foo", "is_str with comment(%s <=> %s)", "foo", "foo");
    is_str("foo", "bar");
    is_str("foo", "bar", "is_str with comment(%s <=> %s)", "foo", "bar");
    isnt_str("foo", "foo");
    isnt_str("foo", "foo", "isnt_str with comment(%s <=> %s)", "foo", "foo");
    isnt_str("foo", "bar");
    isnt_str("foo", "bar", "isnt_str with comment(%s <=> %s)", "foo", "bar");

    is_p(testbuf, testbuf);
    is_p(testbuf, testbuf, "is_p with comment(%s <=> %s)", "testbuf", "testbuf");
    is_p(testbuf, invalidbuf);
    is_p(testbuf, invalidbuf, "is_p with comment(%s <=> %s)", "testbuf", "invalidbuf");
    isnt_p(testbuf, testbuf);
    isnt_p(testbuf, testbuf, "isnt_p with comment(%s <=> %s)", "testbuf", "testbuf");
    isnt_p(testbuf, invalidbuf);
    isnt_p(testbuf, invalidbuf, "isnt_p with comment(%s <=> %s)", "testbuf", "invalidbuf");

    is_mem(testbuf, testbuf, sizeof(testbuf));
    is_mem(testbuf, testbuf, sizeof(testbuf), "is_mem with comment(%s <=> %s)", "testbuf", "testbuf");
    is_mem(testbuf, invalidbuf, sizeof(testbuf));
    is_mem(testbuf, invalidbuf, sizeof(testbuf), "is_mem with comment(%s <=> %s)", "testbuf", "invalidbuf");
    isnt_mem(testbuf, testbuf, sizeof(testbuf));
    isnt_mem(testbuf, testbuf, sizeof(testbuf),  "isnt_mem with comment(%s <=> %s)", "testbuf", "testbuf");
    isnt_mem(testbuf, invalidbuf,  sizeof(testbuf));
    isnt_mem(testbuf, invalidbuf, sizeof(testbuf), "isnt_mem with comment(%s <=> %s)", "testbuf", "invalidbuf");
    

    done_testing(-1);

    return 0;
}
