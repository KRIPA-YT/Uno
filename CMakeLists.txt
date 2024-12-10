# CMakeList.txt : CMake project for Uno, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Uno")

# Add source to this project's executable
include_directories(.)
add_executable (Uno "src/Card.h" "src/Uno.h" "src/Uno.cpp")

set_property(TARGET Uno PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
