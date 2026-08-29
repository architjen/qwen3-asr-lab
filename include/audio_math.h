#pragma once

#include <cstddef>
#include <vector>

namespace asr {

struct SignalStats {
    std::size_t sample_count;
    float peak_absolute;
    double mean;
    double rms;
};

SignalStats analyze_signal(
    const std::vector<float>& samples
);

}  // namespace asr
