find_package(PkgConfig REQUIRED)

pkg_check_modules(Pegmatite REQUIRED pegmatite)
include_directories(${Pegmatite_INCLUDE_DIRS})
link_directories(${Pegmatite_LIBRARY_DIRS})
message("Link dirs: ${Pegmatite_LIBRARY_DIRS}")

set(REASONER_SRC
    AssertedEvidence.cpp
    BackedWME.cpp
    EvidenceComparator.cpp
    InferredEvidence.cpp
    Reasoner.cpp
    RuleGrammar.cpp
    RuleParser.cpp
    RuleParserAST.cpp

    # ebnf parsing
    # parserlib/source/ast.cpp
    # parserlib/source/parser.cpp
)
include_directories(${PROJECT_SOURCE_DIR})

add_library(rete-reasoner SHARED ${REASONER_SRC})
target_link_libraries(rete-reasoner rete-core rete-rdf ${Pegmatite_LIBRARIES})

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-reasoner
    LIBRARY DESTINATION lib
)
