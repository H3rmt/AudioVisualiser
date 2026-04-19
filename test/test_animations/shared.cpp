#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <unity.h>

#include <Animations.hpp>
#include <AnimationsUtil.hpp>

#include "shared.hpp"
#include "../shared.hpp"

#include <string>

const uint32_t padding = AnimationsUtil::ColorH(42);

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

RenderCase makeCase(
    const char *name,
    const Animations::StripData &config,
    const uint32_t lvl,
    std::vector<uint32_t> expected
) {
    std::vector<uint32_t> newExpected(expected.size() + 4);
    newExpected[0] = padding;
    newExpected[1] = padding;
    std::copy(expected.begin(), expected.end(), newExpected.begin() + 2);
    newExpected[expected.size() + 2] = padding;
    newExpected[expected.size() + 3] = padding;
    // expected
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(config.ledCount, newExpected.size() - 4,
                                     (std::string("expected length missmatch: ") + name).c_str());
    return RenderCase{name, config, lvl, newExpected};
}

std::vector<uint32_t> prepare(const RenderCase &testCase) {
    const uint32_t totalBytes = testCase.config.ledCount;
    std::vector<uint32_t> pixels(totalBytes + 4);
    std::fill_n(pixels.data(), totalBytes + 4, 1);
    // pad 2 start and 2 end
    pixels[0] = padding;
    pixels[1] = padding;
    pixels[totalBytes + 2] = padding;
    pixels[totalBytes + 3] = padding;
    return pixels;
}
