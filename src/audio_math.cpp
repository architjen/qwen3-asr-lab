#include "audio_math.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace asr {

SignalStats analyze_signal(
    const std::vector<float>& samples
) {
    if (samples.empty()) {
        throw std::invalid_argument(
            "cannot analyze an empty signal"
        );
    }

    float peak_absolute = 0.0F;
    double sum = 0.0;
    double sum_of_squares = 0.0;

    for (const float sample : samples) {
        const double value =
            static_cast<double>(sample);

        sum += value;
        sum_of_squares += value * value;

        peak_absolute = std::max(
            peak_absolute,
            std::abs(sample)
        );
    }

    const double sample_count =
        static_cast<double>(samples.size());

    return SignalStats{
        samples.size(),
        peak_absolute,
        sum / sample_count,
        std::sqrt(sum_of_squares / sample_count)
    };
}

}  // namespace asr
