cmake_minimum_required(VERSION 2.8)

project(rete)
add_definitions(-std=c++11 -Wall)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

include_directories(include)

set(SRC_FILES
    src/Agenda.cpp
    src/AgendaNode.cpp
    src/AlphaBetaAdapter.cpp
    src/AlphaMemory.cpp
    src/AlphaNode.cpp
    src/AssertedEvidence.cpp
    src/BackedWME.cpp
    src/BetaComparator.cpp
    src/BetaMemory.cpp
    src/BetaNode.cpp
    src/InferredEvidence.cpp
    src/InferTriple.cpp
    src/JoinNode.cpp
    src/Network.cpp
    src/Node.cpp
    src/Production.cpp
    src/ProductionNode.cpp
    src/Reasoner.cpp
    src/Token.cpp
    src/Triple.cpp
    src/TripleAlpha.cpp
    src/TripleConsistency.cpp
    src/TripleJoin.cpp
    src/Util.cpp
    src/WME.cpp
    src/WMEComparator.cpp
)

add_library(rete SHARED ${SRC_FILES})

# add tests
add_executable(AvoidDuplicate_241 test/AvoidDuplicate_241.cpp)
target_link_libraries(AvoidDuplicate_241 rete)
add_test(NAME AvoidDuplicate_241 COMMAND AvoidDuplicate_241)

add_executable(NoDuplicateInAlpha test/NoDuplicateInAlpha.cpp)
target_link_libraries(NoDuplicateInAlpha rete)
add_test(NAME NoDuplicateInAlpha COMMAND NoDuplicateInAlpha)

add_executable(TripleJoinTest test/TripleJoinTest.cpp)
target_link_libraries(TripleJoinTest rete)
add_test(NAME TripleJoinTest COMMAND TripleJoinTest)

add_executable(test_rete test/main.cpp)
target_link_libraries(test_rete rete)
add_test(NAME main COMMAND test_rete)

enable_testing()
