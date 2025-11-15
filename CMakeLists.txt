cmake_minimum_required(VERSION 3.20)
project(arkanoid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES arm64)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

add_compile_definitions(SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders")

set(SOURCES
    src/main.cpp
    src/projector_cpu_depthsort.cpp
    src/renderer_gl41.cpp
    src/orbit.cpp
    src/ply_3dgs.cpp
    src/meshgen.cpp
    src/prim.cpp
    # src/arkanoid.cpp
    # src/3DGS.cpp
)

add_executable(main ${SOURCES})

target_compile_definitions(main PRIVATE GLM_ENABLE_EXPERIMENTAL)

if(APPLE)
    set(CMAKE_OSX_ARCHITEXTURES "arm64" CACHE STRING "Build architectures" FORCE)

    target_link_libraries(main
        PRIVATE
            glfw
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )   
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)

    target_include_directories(main PRIVATE
        ${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIR}
    )

    target_link_libraries(main PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
    )
endif()

target_include_directories(main PRIVATE
    src
    ${GLM_INCLUDE_DIRS}
)
