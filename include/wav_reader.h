#pragma once

#include <cstdint>
#include <filesystem>

namespace asr {

struct WavInfo {
    std::uint16_t audio_format;
    std::uint16_t channels;
    std::uint32_t sample_rate;
    std::uint16_t bits_per_sample;
    std::uint32_t data_bytes;
    double duration_seconds;
};

WavInfo inspect_wav(const std::filesystem::path& path);

}  // namespace asr