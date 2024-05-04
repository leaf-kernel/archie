cmake_minimum_required(VERSION 3.13)
project(Archie)

# Enable AddressSanitizer
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

file(GLOB_RECURSE CC_FILES
    "src/*.c"
    "src/*.cpp"
)

add_executable(archie ${CC_FILES})
target_link_options(archie PUBLIC -fsanitize=address)
