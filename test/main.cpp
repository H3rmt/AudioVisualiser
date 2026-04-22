#include <unity.h>

#include "tests.hpp"

void setUp() {
}

void tearDown() {
}

int main(const int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_Color);
    RUN_TEST(test_renderNormal);
    RUN_TEST(test_renderNormalColored);
    // RUN_TEST(test_renderCircle);
    return UNITY_END();
}
