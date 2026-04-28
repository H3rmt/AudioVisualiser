#pragma once

constexpr uint16_t divider_min = 5;
constexpr uint16_t divider_max = 40;

#define OFF_SECONDS 30

// #define FLOATING_AVG_MIN_BASE 2000
// added / 10 because divider is *10
#define FLOATING_AVG_MIN_BASE 300

// constexpr int maxAvgFreq = 45;
constexpr int maxAvgFreq = 60;
