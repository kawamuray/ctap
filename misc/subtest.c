#include "ctap.h"

void test_subtest(void)
{
    ok(1);

    is_int(1, 1, "is_int with comment(%d <=> %d)", 1, 1);
    is_double(1.0, 1.0);
}

void test_subtest_fail(void)
{
    plan(-1);

    ok(1);

    is_int(1, 1, "is_int with comment(%d <=> %d)", 1, 1);
    is_double(1.0, 0.0);
}

int main(void)
{
    plan(2);

    subtest("success", test_subtest);
    subtest("failed", test_subtest_fail);

    return 0;
}
