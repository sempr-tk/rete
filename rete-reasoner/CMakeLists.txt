find_package(PkgConfig REQUIRED)

pkg_check_modules(Pegmatite REQUIRED pegmatite)
include_directories(${Pegmatite_INCLUDE_DIRS})
link_directories(${Pegmatite_LIBRARY_DIRS})
find_package(nlohmann_json REQUIRED) # apt install nlohmann-json-dev

set(REASONER_SRC
    Argument.cpp
    AssertedEvidence.cpp
    BackedWME.cpp
    EvidenceComparator.cpp
    ExplanationToDotVisitor.cpp
    ExplanationToJSONVisitor.cpp
    InferenceState.cpp
    InferredEvidence.cpp
    ParsedRule.cpp
    Reasoner.cpp
    RuleGrammar.cpp
    RuleParser.cpp
    RuleParserAST.cpp
    MakeSkolemBuilder.cpp
    NodeBuilder.cpp
    NodeBuilderHelper.cpp
    TripleConditionBuilder.cpp
    TripleEffectBuilder.cpp
    TrueNodeBuilder.cpp
    UtilBuiltinBuilder.cpp

    # ebnf parsing
    # parserlib/source/ast.cpp
    # parserlib/source/parser.cpp
)
include_directories(${PROJECT_SOURCE_DIR})

add_library(rete-reasoner SHARED ${REASONER_SRC})
set_target_properties(rete-reasoner PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
target_link_libraries(rete-reasoner rete-core rete-rdf ${Pegmatite_LIBRARIES})

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    COMPONENT reasoner
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-reasoner
    LIBRARY DESTINATION lib COMPONENT reasoner
)
