cmake_minimum_required(VERSION 3.20)

project(qwen3_asr_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(audio_math
    src/audio_math.cpp
)

target_compile_options(audio_math PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
)
