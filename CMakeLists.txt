set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/_Projects/Output")

set(SOURCE_FILES main.c Map.h List.c List.h status.c status.h Map.c)
add_executable(course4_project ${SOURCE_FILES})

#add_definitions(-DDEBUG)