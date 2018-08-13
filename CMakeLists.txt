cmake_minimum_required(VERSION 2.8)

project(rete)
add_definitions(-std=c++11 -Wall)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

include_directories(include)

set(SRC_FILES
    src/AlphaBetaAdapter.cpp
    src/AlphaMemory.cpp
    src/AlphaNode.cpp
    src/BetaMemory.cpp
    src/BetaNode.cpp
    src/JoinNode.cpp
    src/Network.cpp
    src/Token.cpp
    src/Triple.cpp
    src/TripleAlpha.cpp
)

add_library(rete SHARED ${SRC_FILES})


add_executable(test_rete test/main.cpp)
target_link_libraries(test_rete rete)
