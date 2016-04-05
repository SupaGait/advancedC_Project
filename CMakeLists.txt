project(AdvancedC_Project)
cmake_minimum_required(VERSION 3.3)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/_Projects/Output")

set(SOURCE_FILES main.c Map.h List.c List.h status.c status.h Map.c)
add_executable(advancedC_Project ${SOURCE_FILES})

set(SOURCE_FILES ListTest.c List.c List.h status.c status.h)
add_executable(listTest ${SOURCE_FILES})