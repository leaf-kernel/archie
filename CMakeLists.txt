cmake_minimum_required(VERSION 3.13)
project(Archie)

file(GLOB_RECURSE CC_FILES
    "src/*.c"
    "src/*.cpp"
)


add_executable(archie ${CC_FILES})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")