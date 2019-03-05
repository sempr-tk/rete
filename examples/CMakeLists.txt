include_directories(${PROJECT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(Pegmatite REQUIRED pegmatite)
include_directories(${Pegmatite_INCLUDE_DIRS})
link_directories(${Pegmatite_LIBRARY_DIRS})

add_executable(GoodAndBadRules GoodAndBadRules.cpp)
target_link_libraries(GoodAndBadRules rete-core rete-rdf rete-reasoner)

add_executable(SubClassOfExample SubClassOfExample.cpp)
target_link_libraries(SubClassOfExample rete-core rete-rdf rete-reasoner)

add_executable(Blocksworld Blocksworld.cpp)
target_link_libraries(Blocksworld rete-core rete-rdf rete-reasoner)

add_executable(Blockstower Blockstower.cpp)
target_link_libraries(Blockstower rete-core rete-rdf rete-reasoner)
