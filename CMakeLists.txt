CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

project("mech" CXX)
SET(NAME "mech")

find_program(CCACHE ccache)
if(CCACHE)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
  message("ccache cannot be found")
endif()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES("src/ZD/" "src/")

ADD_DEFINITIONS(-DNAME="${NAME}")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -fno-omit-frame-pointer -Wall -Wextra -Wpedantic -DDEBUG")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -O3 -s -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
add_compile_options(${CMAKE_CXX_FLAGS} -fPIC -std=c++20 -Wall)

FILE(GLOB SOURCES "src/*.cpp" "src/ZD/*.cpp" "src/3rd/imgui/*.cpp")
ADD_EXECUTABLE(${NAME} "${SOURCES}")
TARGET_LINK_LIBRARIES(${NAME} PRIVATE stdc++ GL GLEW glfw pthread)

add_custom_target(run
  COMMAND build/${NAME}
  DEPENDS ${NAME}
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
