cmake_minimum_required(VERSION 3.15)
project(OrbitalSaveSystemTests LANGUAGES CXX)

add_executable(save_system_tests SaveSystemTests.cpp)
target_compile_features(save_system_tests PRIVATE cxx_std_17)
target_include_directories(save_system_tests PRIVATE ../include/orbital)

enable_testing()
add_test(NAME save_system_tests COMMAND save_system_tests)
