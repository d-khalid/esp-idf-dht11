cmake_minimum_required(VERSION 3.16)

project(esp-idf-dht11)

add_library(esp-idf-dht11 src/esp-idf-dht11.c)

target_include_directories(esp-idf-dht11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Follow C++11
target_compile_features(mylibrary PUBLIC cxx_std_11)


