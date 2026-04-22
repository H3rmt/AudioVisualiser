#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <unity.h>

#include <Animations.hpp>
#include <AnimationsUtil.hpp>

#include "shared.hpp"

void test_renderNormal() {
    const auto cases = std::array{
        makeCase(
            "clears all pixels at zero level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            0,
            std::vector{
                black, black, black, black, black, black, black, black, black, black
            }
        ),
        makeCase(
            "clears all pixels at zero level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            0,
            std::vector{
                black, black, black, black, black, black, black, black, black, black, black
            }
        ),
        makeCase(
            "fill all pixels at full level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            UINT32_MAX,
            std::vector{
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
            }
        ),
        makeCase(
            "fill all pixels at full level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX,
            std::vector{
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
                AnimationsUtil::ColorH(UINT16_MAX)
            }
        ),
        makeCase(
            "fill half pixels at half level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            UINT32_MAX / 2,
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black,
                black
            }
        ),
        makeCase(
            "fill half pixels at half+1 level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 + 1,
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black
            }
        ),
        makeCase(
            "fill half pixels at half-1 level 10 items",
            Animations::StripData{10, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 - 1,
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black,
                black
            }
        ),
        makeCase(
            "fill half pixels at half level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2,
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black,
                black
            }
        ),
        makeCase(
            "fill half pixels at half-1 level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 - 1,
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black,
                black
            }
        ),
        makeCase(
            "fill half pixels at half+1 level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 + 1,
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black,
                black
            }
        ),
        makeCase(
            "fill half pixels at half+x level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 + (UINT32_MAX / 21),
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black
            }
        ),
        makeCase(
            "fill half pixels at half-x level 11 items",
            Animations::StripData{11, false, false, false, 0, 0, 0},
            UINT32_MAX / 2 - (UINT32_MAX / 22),
            std::vector{
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                AnimationsUtil::ColorH(UINT16_MAX),
                black,
                black,
                black,
                black,
                black,
                black,
                black
            }
        ),
    };

    for (const auto &testCase: cases) {
        std::printf("[normal] case: %s (%" PRIu32 "/%" PRIu32 " = %d%% = [%d/%d])\n", testCase.name, testCase.lvl,
            UINT32_MAX,
            AnimationsUtil::map(testCase.lvl, UINT32_MAX, 100),
            AnimationsUtil::map(testCase.lvl, UINT32_MAX, testCase.config.ledCount), testCase.config.ledCount
        );
        auto elements = prepare(testCase);
        printPixels(elements);
        Animations::renderNormal(&elements[2], &testCase.config, testCase.lvl);
        printPixels(elements);

        printPixels(testCase.expected);
        for (size_t i = 0; i < testCase.config.ledCount; ++i) {
            TEST_ASSERT_EQUAL_UINT32_MESSAGE(testCase.expected[i], elements[i], testCase.name);
        }
        std::printf("\n");
    }
}
