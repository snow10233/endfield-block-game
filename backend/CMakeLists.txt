cmake_minimum_required(VERSION 3.28)
project(EndfieldBlockGame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(core src/board.cpp)
target_include_directories(core PUBLIC include/)
target_compile_features(core PUBLIC cxx_std_20)

# 將 tests 資料夾完整複製到編譯輸出的目錄下
# 僅測試階段用 正式發布後拔掉
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE core)
