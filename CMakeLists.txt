# Projects Settings
cmake_minimum_required (VERSION 3.1)
project (opengl-skeleton)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

# The main executable
add_executable(main
    ${SRC_FILES}
)
set_property(TARGET main PROPERTY CXX_STANDARD 11)
target_compile_options(main PRIVATE -Wall)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(main PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(main PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(main PRIVATE glm)

# sfml
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(lib/sfml EXCLUDE_FROM_ALL)
target_link_libraries(main PRIVATE sfml-graphics sfml-audio)

#target_link_libraries(${PROJECT_NAME} sfml-graphics)
#target_link_libraries(${PROJECT_NAME} sfml-audio)
