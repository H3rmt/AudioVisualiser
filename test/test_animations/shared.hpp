#pragma once
#include <cstdint>
#include <vector>

constexpr uint32_t black = 0;

struct RenderCase {
    const char *name;
    Animations::StripData config;
    uint32_t lvl;
    std::vector<uint32_t> expected;
};


void printPixels(const std::vector<uint32_t> &pixels);

RenderCase makeCase(
    const char *name,
    const Animations::StripData &config,
    const uint32_t lvl,
    std::vector<uint32_t> expected
);


std::vector<uint32_t> prepare(const RenderCase &testCase);


Animations::StripData createStripData();