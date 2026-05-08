cmake_minimum_required(VERSION 3.28)

project(EndfieldBlockGame CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
  main.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(endfield ${SOURCES})

if(MSVC)
    target_compile_options(endfield PRIVATE /utf-8)
endif()
