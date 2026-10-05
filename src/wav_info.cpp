#include "wav_reader.h"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: wav_info <path>\n";
        return 2;
    }

    try {
        const asr::WavInfo info = asr::inspect_wav(argv[1]);

        std::cout << "audio_format=" << info.audio_format << '\n';
        std::cout << "channels=" << info.channels << '\n';
        std::cout << "sample_rate=" << info.sample_rate << '\n';
        std::cout << "bits_per_sample=" << info.bits_per_sample << '\n';
        std::cout << "data_bytes=" << info.data_bytes << '\n';
        std::cout << "duration_seconds="
                  << info.duration_seconds << '\n';

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}