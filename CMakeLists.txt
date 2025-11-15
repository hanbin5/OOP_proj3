cmake_minimum_required(VERSION 3.20)
project(arkanoid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES arm64)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

set(SOURCES
    src/main.cpp
)

add_executable(main ${SOURCES})

target_compile_definitions(main PRIVATE GLM_ENABLE_EXPERIMENTAL)

if(APPLE)
    set(CMAKE_OSX_ARCHITEXTURES "arm64" CACHE STRING "Build architectures" FORCE)

    target_link_libraries(main
        PRIVATE
            "-framework OpenGL"
            "-framework GLUT"
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
