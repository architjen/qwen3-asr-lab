#include "audio_math.h"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

int failure_count = 0;

void expect_near(
    const double actual,
    const double expected,
    const double tolerance,
    const std::string_view test_name
) {
    if (std::abs(actual - expected) > tolerance) {
        std::cerr
            << "FAIL: " << test_name
            << " expected=" << expected
            << " actual=" << actual
            << '\n';

        ++failure_count;
    }
}

void expect_equal(
    const std::size_t actual,
    const std::size_t expected,
    const std::string_view test_name
) {
    if (actual != expected) {
        std::cerr
            << "FAIL: " << test_name
            << " expected=" << expected
            << " actual=" << actual
            << '\n';

        ++failure_count;
    }
}

void expect_true(
    const bool condition,
    const std::string_view test_name
) {
    if (!condition) {
        std::cerr
            << "FAIL: " << test_name
            << '\n';

        ++failure_count;
    }
}

}  // namespace

int main() {
    const std::vector<float> constant_signal{
        0.25F, 0.25F, 0.25F, 0.25F
    };

    const asr::SignalStats constant_stats =
        asr::analyze_signal(constant_signal);

    expect_equal(
        constant_stats.sample_count,
        4,
        "constant sample count"
    );

    expect_near(
        constant_stats.peak_absolute,
        0.25,
        1e-6,
        "constant peak"
    );

    expect_near(
        constant_stats.mean,
        0.25,
        1e-6,
        "constant mean"
    );

    expect_near(
        constant_stats.rms,
        0.25,
        1e-6,
        "constant RMS"
    );

    const std::vector<float> alternating_signal{
        -1.0F, 1.0F, -1.0F, 1.0F
    };

    const asr::SignalStats alternating_stats =
        asr::analyze_signal(alternating_signal);

    expect_near(
        alternating_stats.peak_absolute,
        1.0,
        1e-6,
        "alternating peak"
    );

    expect_near(
        alternating_stats.mean,
        0.0,
        1e-6,
        "alternating mean"
    );

    expect_near(
        alternating_stats.rms,
        1.0,
        1e-6,
        "alternating RMS"
    );

    const std::vector<float> empty_signal;

    bool empty_signal_rejected = false;

    try {
        asr::analyze_signal(empty_signal);
    } catch (const std::invalid_argument&) {
        empty_signal_rejected = true;
    }

    expect_true(
        empty_signal_rejected,
        "empty signal rejected"
    );

    if (failure_count == 0) {
        std::cout << "All audio math tests passed\n";
        return 0;
    }

    std::cerr
        << failure_count
        << " test(s) failed\n";

    return 1;
}
