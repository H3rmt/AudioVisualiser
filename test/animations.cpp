#include <algorithm>
#include <array>
#include <cstdio>
#include <cinttypes>
#include <cstdint>
#include <vector>
#include <unity.h>
#include <AnimationsUtil.hpp>
#include <Animations.hpp>

#include "../lib/Animations/src/Animations.hpp"
#include "../lib/Animations/src/AnimationsUtil.hpp"

const uint32_t padding = AnimationsUtil::ColorH(1);
constexpr uint32_t black = 0;

namespace {
    struct RenderNormal2Case {
        const char *name;
        Animations::StripData config;
        uint32_t lvl;
        std::vector<uint32_t> expected;
    };

    uint16_t ColorToHue(uint32_t color) {
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;

        uint32_t hue1530 = 0;

        if (r == 255 && b == 0) {
            hue1530 = g; // Red to Yellow (0-254)
        } else if (g == 255 && b == 0) {
            hue1530 = 510 - r; // Yellow to Green (255-509)
        } else if (g == 255 && r == 0) {
            hue1530 = 510 + b; // Green to Cyan (510-764)
        } else if (b == 255 && r == 0) {
            hue1530 = 1020 - g; // Cyan to Blue (765-1019)
        } else if (b == 255 && g == 0) {
            hue1530 = 1020 + r; // Blue to Magenta (1020-1274)
        } else if (r == 255 && g == 0) {
            hue1530 = 1530 - b; // Magenta to Red (1275-1530)
        }

        // Scale back to 0-65535: (hue1530 * 65536) / 1530
        return static_cast<uint16_t>((hue1530 * 65536L) / 1530L);
    }

    void printPixels(const std::vector<uint32_t> &pixels) {
        std::printf("pixels (%lu): ", pixels.size());
        for (size_t i = 0; i < pixels.size(); ++i) {
            std::printf("%u(%" PRIu32 ")", ColorToHue(pixels[i]), pixels[i]);
            if (i + 1 < pixels.size()) {
                std::printf(", ");
            }
        }
        std::printf("\n");
    }

    RenderNormal2Case makeCase(
        const char *name,
        const Animations::StripData &config,
        const uint32_t lvl,
        std::vector<uint32_t> expected
    ) {
        return RenderNormal2Case{name, config, lvl, expected};
    }

    void runCase(const RenderNormal2Case &testCase) {
        const uint32_t totalBytes = testCase.config.ledCount;
        std::vector<uint32_t> pixels(totalBytes + 4);
        std::fill_n(pixels.data(), totalBytes + 4, 1);
        // pad 2 start and 2 end
        pixels[0] = padding;
        pixels[1] = padding;
        pixels[totalBytes + 2] = padding;
        pixels[totalBytes + 3] = padding;

        std::printf("case: %s (%" PRIu32 "/%" PRIu32 " = %d%% = [%d/%d])\n", testCase.name, testCase.lvl, UINT32_MAX,
                    AnimationsUtil::map(testCase.lvl, UINT32_MAX, 100),
                    AnimationsUtil::map(testCase.lvl, UINT32_MAX, testCase.config.ledCount), testCase.config.ledCount
        );
        printPixels(pixels);
        Animations::renderNormal(&pixels[2], &testCase.config, testCase.lvl);
        printPixels(pixels);

        printPixels(testCase.expected);
        for (size_t i = 0; i < totalBytes; ++i) {
            // TEST_ASSERT_EQUAL_UINT16_MESSAGE(ColorToHue(testCase.expected[i]), ColorToHue(pixels[i]), testCase.name);
            TEST_ASSERT_EQUAL_UINT32_MESSAGE(testCase.expected[i], pixels[i], testCase.name);
        }
        std::printf("\n");
    }
}

void setUp() {
}

void tearDown() {
}

void test_ColorH() {
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

void test_renderNormal2_cases() {
    const auto cases = std::array{
        makeCase(
            "clears all pixels at zero level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            0,
            std::vector{
                padding,
                padding,
                black, black, black, black, black, black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "clears all pixels at zero level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            0,
            std::vector{
                padding,
                padding,
                black, black, black, black, black, black, black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "fill all pixels at full level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            UINT32_MAX,
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                padding,
                padding
            }
        ),
        makeCase(
            "fill all pixels at full level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX,
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                padding,
                padding
            }
        ),
        makeCase(
            "fill half pixels at half level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            UINT32_MAX / 2,
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black, black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "fill half pixels at half+1 level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 + 1,
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "fill half pixels at half level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2,
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black, black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "fill half pixels at half-1 level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 - 1,
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black, black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "fill half pixels at half+1 level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 + 1,
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black, black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "fill half pixels at half+x level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 + (UINT32_MAX / 21),
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black, black, black, black, black,
                padding,
                padding
            }
        ),
        makeCase(
            "fill half pixels at half-x level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 - (UINT32_MAX / 22),
            std::vector{
                padding,
                padding,
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black, black, black, black, black, black, black,
                padding,
                padding
            }
        ),
    };

    for (const auto &testCase: cases) {
        runCase(testCase);
    }
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    UNITY_BEGIN();
    RUN_TEST(test_ColorH);
    RUN_TEST(test_renderNormal2_cases);
    return UNITY_END();
}
