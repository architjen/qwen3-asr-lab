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
        static_cast<std::size_t>(sample_rate * duration_seconds);

    std::vector<float> samples(sample_count);

    double sum_of_squares = 0.0;

    for (std::size_t i = 0; i < samples.size(); ++i) {
        const double time_seconds =
            static_cast<double>(i) /
            static_cast<double>(sample_rate);

        samples[i] = static_cast<float>(
            amplitude *
            std::sin(2.0 * pi * frequency_hz * time_seconds)
        );

        const double sample = static_cast<double>(samples[i]);
        sum_of_squares += sample * sample;
    }

    const double measured_duration_seconds =
        static_cast<double>(samples.size()) /
        static_cast<double>(sample_rate);

    const double rms =
        std::sqrt(sum_of_squares /
                  static_cast<double>(samples.size()));

    std::cout << "sample_rate=" << sample_rate << '\n';
    std::cout << "sample_count=" << samples.size() << '\n';
    std::cout << "duration_seconds="
              << measured_duration_seconds << '\n';
    std::cout << "rms=" << rms << '\n';

    return 0;
}
