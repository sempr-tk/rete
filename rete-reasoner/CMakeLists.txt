set(REASONER_SRC
    AssertedEvidence.cpp
    BackedWME.cpp
    EvidenceComparator.cpp
    InferredEvidence.cpp
    Reasoner.cpp
    RuleParser.cpp
    RuleParserAST.cpp

    # ebnf parsing
    parserlib/source/ast.cpp
    parserlib/source/parser.cpp
)
include_directories(${PROJECT_SOURCE_DIR})

add_library(rete-reasoner SHARED ${REASONER_SRC})
target_link_libraries(rete-reasoner rete-core rete-rdf)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-reasoner
    LIBRARY DESTINATION lib
)
