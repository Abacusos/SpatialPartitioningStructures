# CMakeList.txt : CMake project for KDTree, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_executable(Tests "Test.cpp" "KDTree.cpp")
target_include_directories(Tests PUBLIC ../Shared)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Tests PROPERTY CXX_STANDARD 17)
endif()

target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(Tests)