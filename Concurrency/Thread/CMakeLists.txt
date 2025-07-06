cmake_minimum_required(VERSION 3.15)
project(Concurrency)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find cpr installed by vcpkg
find_package(cpr CONFIG REQUIRED)

add_executable(my_program main.cpp)

# Link your program with cpr
target_link_libraries(my_program PRIVATE cpr::cpr)
target_compile_options(my_program PRIVATE -fexperimental-library)

# ✅ cmake_minimum_required
# ✅ project()
# ✅ set(CMAKE_CXX_STANDARD)
# ✅ find_package()   # for libraries you installed with vcpkg
# ✅ add_executable()
# ✅ target_link_libraries()
