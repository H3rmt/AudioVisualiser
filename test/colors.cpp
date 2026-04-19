#include <algorithm>
#include <cstdio>
#include <cinttypes>
#include <unity.h>
#include <AnimationsUtil.hpp>

#include "shared.hpp"

void test_Color() {
    const auto color = AnimationsUtil::ColorH(0);
    std::printf("color: %" PRIu32 "\n", color);
    TEST_ASSERT_EQUAL_UINT16(0, ColorToHue(color));
    const auto color1 = AnimationsUtil::ColorH(5000);
    std::printf("color1: %" PRIu32 "\n", color1);
    std::printf("color1: %d\n", ColorToHue(color1));
    TEST_ASSERT_GREATER_THAN_UINT16(4900, ColorToHue(color1));
    TEST_ASSERT_LESS_THAN_UINT16(5100, ColorToHue(color1));
    const auto color2 = AnimationsUtil::ColorH(10000);
    std::printf("color2: %" PRIu32 "\n", color2);
    std::printf("color2: %d\n", ColorToHue(color2));
    TEST_ASSERT_GREATER_THAN_UINT16(9900, ColorToHue(color2));
    TEST_ASSERT_LESS_THAN_UINT16(10100, ColorToHue(color2));
    const auto color3 = AnimationsUtil::ColorH(20000);
    std::printf("color3: %" PRIu32 "\n", color3);
    std::printf("color3: %d\n", ColorToHue(color3));
    TEST_ASSERT_GREATER_THAN_UINT16(19900, ColorToHue(color3));
    TEST_ASSERT_LESS_THAN_UINT16(20100, ColorToHue(color3));
}