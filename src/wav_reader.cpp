#include "wav_reader.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace asr {
namespace {

using Tag = std::array<char, 4>;

void read_exact(
    std::istream& input,
    char* destination,
    std::streamsize byte_count,
    std::string_view description
) {
    input.read(destination, byte_count);

    if (input.gcount() != byte_count) {
        throw std::runtime_error(
            "truncated WAV while reading " +
            std::string(description)
        );
    }
}

Tag read_tag(
    std::istream& input,
    std::string_view description
) {
    Tag tag{};

    read_exact(
        input,
        tag.data(),
        static_cast<std::streamsize>(tag.size()),
        description
    );

    return tag;
}

bool tag_equals(
    const Tag& tag,
    std::string_view expected
) {
    return expected.size() == tag.size() &&
           std::string_view(tag.data(), tag.size()) == expected;
}

std::uint16_t read_u16_le(std::istream& input) {
    std::array<unsigned char, 2> bytes{};

    read_exact(
        input,
        reinterpret_cast<char*>(bytes.data()),
        static_cast<std::streamsize>(bytes.size()),
        "16-bit integer"
    );

    return
        static_cast<std::uint16_t>(bytes[0]) |
        static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(bytes[1]) << 8
        );
}

std::uint32_t read_u32_le(std::istream& input) {
    std::array<unsigned char, 4> bytes{};

    read_exact(
        input,
        reinterpret_cast<char*>(bytes.data()),
        static_cast<std::streamsize>(bytes.size()),
        "32-bit integer"
    );

    return
        static_cast<std::uint32_t>(bytes[0]) |
        (static_cast<std::uint32_t>(bytes[1]) << 8) |
        (static_cast<std::uint32_t>(bytes[2]) << 16) |
        (static_cast<std::uint32_t>(bytes[3]) << 24);
}

std::uint64_t tell_position(std::istream& input) {
    const std::streampos position = input.tellg();

    if (position == std::streampos(-1)) {
        throw std::runtime_error(
            "could not determine WAV file position"
        );
    }

    const std::streamoff offset =
        static_cast<std::streamoff>(position);

    if (offset < 0) {
        throw std::runtime_error(
            "WAV file position is negative"
        );
    }

    return static_cast<std::uint64_t>(offset);
}

void seek_absolute(
    std::istream& input,
    std::uint64_t position
) {
    const auto maximum_offset =
        static_cast<std::uint64_t>(
            std::numeric_limits<std::streamoff>::max()
        );

    if (position > maximum_offset) {
        throw std::runtime_error(
            "WAV position is too large"
        );
    }

    input.seekg(
        static_cast<std::streamoff>(position),
        std::ios::beg
    );

    if (!input) {
        throw std::runtime_error(
            "could not seek within WAV file"
        );
    }
}

std::uint64_t determine_file_size(std::ifstream& input) {
    input.seekg(0, std::ios::end);

    if (!input) {
        throw std::runtime_error(
            "could not seek to the end of WAV file"
        );
    }

    const std::uint64_t size = tell_position(input);

    input.seekg(0, std::ios::beg);

    if (!input) {
        throw std::runtime_error(
            "could not return to the start of WAV file"
        );
    }

    return size;
}

}  // namespace

WavInfo inspect_wav(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);

    if (!input) {
        throw std::runtime_error(
            "could not open file: " + path.string()
        );
    }

    const std::uint64_t physical_file_size =
        determine_file_size(input);

    if (physical_file_size < 12) {
        throw std::runtime_error(
            "file is too small to be a WAV file"
        );
    }

    const Tag riff_tag = read_tag(input, "RIFF identifier");

    if (!tag_equals(riff_tag, "RIFF")) {
        throw std::runtime_error(
            "file does not begin with RIFF"
        );
    }

    const std::uint32_t riff_size = read_u32_le(input);

    const Tag wave_tag = read_tag(input, "WAVE identifier");

    if (!tag_equals(wave_tag, "WAVE")) {
        throw std::runtime_error(
            "RIFF file is not a WAVE file"
        );
    }

    // RIFF size counts everything after the first eight bytes.
    const std::uint64_t riff_end =
        8ULL + static_cast<std::uint64_t>(riff_size);

    if (riff_end < 12) {
        throw std::runtime_error(
            "invalid RIFF size"
        );
    }

    if (riff_end > physical_file_size) {
        throw std::runtime_error(
            "declared RIFF size exceeds physical file size"
        );
    }

    WavInfo info{};
    bool found_format = false;
    bool found_data = false;

    std::uint16_t block_alignment = 0;
    std::uint32_t byte_rate = 0;

    while (!found_format || !found_data) {
        const std::uint64_t chunk_header_start =
            tell_position(input);

        // Each chunk needs an eight-byte header.
        if (chunk_header_start + 8ULL > riff_end) {
            break;
        }

        const Tag chunk_id =
            read_tag(input, "chunk identifier");

        const std::uint32_t chunk_size =
            read_u32_le(input);

        const std::uint64_t payload_start =
            tell_position(input);

        const std::uint64_t payload_end =
            payload_start +
            static_cast<std::uint64_t>(chunk_size);

        const std::uint64_t padded_end =
            payload_end +
            static_cast<std::uint64_t>(chunk_size & 1U);

        if (payload_end < payload_start ||
            padded_end < payload_end ||
            padded_end > riff_end) {
            throw std::runtime_error(
                "chunk exceeds RIFF boundary"
            );
        }

        if (tag_equals(chunk_id, "fmt ") &&
            !found_format) {
            if (chunk_size < 16) {
                throw std::runtime_error(
                    "fmt chunk is smaller than 16 bytes"
                );
            }

            info.audio_format = read_u16_le(input);
            info.channels = read_u16_le(input);
            info.sample_rate = read_u32_le(input);
            byte_rate = read_u32_le(input);
            block_alignment = read_u16_le(input);
            info.bits_per_sample = read_u16_le(input);

            found_format = true;

            // Skip optional extension bytes and odd padding.
            seek_absolute(input, padded_end);
        } else if (
            tag_equals(chunk_id, "data") &&
            !found_data
        ) {
            info.data_bytes = chunk_size;
            found_data = true;

            // If fmt has not appeared yet, continue scanning.
            if (!found_format) {
                seek_absolute(input, padded_end);
            }
        } else {
            // Unknown chunks are valid. Skip them safely.
            seek_absolute(input, padded_end);
        }
    }

    if (!found_format) {
        throw std::runtime_error(
            "WAV file has no fmt chunk"
        );
    }

    if (!found_data) {
        throw std::runtime_error(
            "WAV file has no data chunk"
        );
    }

    if (info.audio_format != 1) {
        throw std::runtime_error(
            "only integer PCM WAV is supported"
        );
    }

    if (info.channels == 0) {
        throw std::runtime_error(
            "channel count must be positive"
        );
    }

    if (info.sample_rate == 0) {
        throw std::runtime_error(
            "sample rate must be positive"
        );
    }

    if (info.bits_per_sample == 0 ||
        info.bits_per_sample % 8 != 0) {
        throw std::runtime_error(
            "bits per sample must be a positive multiple of 8"
        );
    }

    const std::uint64_t bytes_per_sample =
        static_cast<std::uint64_t>(
            info.bits_per_sample / 8
        );

    const std::uint64_t expected_block_alignment =
        static_cast<std::uint64_t>(info.channels) *
        bytes_per_sample;

    const std::uint64_t expected_byte_rate =
        static_cast<std::uint64_t>(info.sample_rate) *
        expected_block_alignment;

    if (expected_block_alignment != block_alignment) {
        throw std::runtime_error(
            "invalid PCM block alignment"
        );
    }

    if (expected_byte_rate != byte_rate) {
        throw std::runtime_error(
            "invalid PCM byte rate"
        );
    }

    if (expected_block_alignment == 0 ||
        info.data_bytes % expected_block_alignment != 0) {
        throw std::runtime_error(
            "PCM data does not contain complete sample frames"
        );
    }

    info.duration_seconds =
        static_cast<double>(info.data_bytes) /
        static_cast<double>(expected_byte_rate);

    return info;
}

}  // namespace asr