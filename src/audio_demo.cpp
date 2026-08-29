#include "audio_math.h"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

int main() {
    constexpr int sample_rate = 16000;
    constexpr double duration_seconds = 1.0;
    constexpr double frequency_hz = 440.0;
    constexpr double amplitude = 0.5;
    constexpr double pi = 3.14159265358979323846;

    const std::size_t sample_count =
        static_cast<std::size_t>(
            sample_rate * duration_seconds
        );

    std::vector<float> samples(sample_count);

    for (std::size_t i = 0;
         i < samples.size();
         ++i) {
        const double time_seconds =
            static_cast<double>(i) /
            static_cast<double>(sample_rate);

        samples[i] = static_cast<float>(
            amplitude *
            std::sin(
                2.0 * pi *
                frequency_hz *
                time_seconds
            )
        );
    }

    const asr::SignalStats stats =
        asr::analyze_signal(samples);

    const double measured_duration_seconds =
        static_cast<double>(stats.sample_count) /
        static_cast<double>(sample_rate);

    std::cout
        << "sample_rate=" << sample_rate << '\n'
        << "sample_count=" << stats.sample_count << '\n'
        << "duration_seconds="
        << measured_duration_seconds << '\n'
        << "peak_absolute="
        << stats.peak_absolute << '\n'
        << "mean=" << stats.mean << '\n'
        << "rms=" << stats.rms << '\n';

    return 0;
}
