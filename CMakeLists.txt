cmake_minimum_required(VERSION 3.21)
project(libdia_unload_example)

set(CMAKE_CXX_STANDARD 14)

include_directories("$ENV{VSINSTALLDIR}/DIA SDK/include")

add_executable(libdia_unload_example
        main.cpp
        dll_ref_counter.cpp
        dll_ref_counter.h)

set(LIB_DIA_GUIDS "$ENV{VSINSTALLDIR}/DIA SDK/lib/amd64/diaguids.lib")

target_link_libraries(libdia_unload_example ntdll.lib "${LIB_DIA_GUIDS}")