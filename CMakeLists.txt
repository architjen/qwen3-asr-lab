cmake_minimum_required(VERSION 3.20)

project(qwen3_asr_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(audio_math_lib STATIC
    src/audio_math.cpp
)

target_include_directories(audio_math_lib PUBLIC
    include
)

target_compile_options(audio_math_lib PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
)

add_executable(audio_math
    src/audio_demo.cpp
)

target_link_libraries(audio_math PRIVATE
    audio_math_lib
)

target_compile_options(audio_math PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
)

enable_testing()

add_executable(audio_math_test
    tests/audio_math_test.cpp
)

target_link_libraries(audio_math_test PRIVATE
    audio_math_lib
)

target_compile_options(audio_math_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
)

add_test(
    NAME audio_math_test
    COMMAND audio_math_test
)

add_library(wav_reader_lib STATIC
    src/wav_reader.cpp
)

target_include_directories(wav_reader_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(wav_reader_lib PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
)

add_executable(wav_info
    src/wav_info.cpp
)

target_link_libraries(wav_info PRIVATE
    wav_reader_lib
)