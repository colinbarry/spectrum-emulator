#include <picotest/picotest.h>
#include <stdio.h>
#include <stdlib.h>

IMPORT_SUITE(keyboard);

int main(int argc, char **argv)
{
    BEGIN_TESTS();
    RUN_SUITE(keyboard);

    printf("\n%i PASSED\n%i FAILED\n", passes, failures);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
