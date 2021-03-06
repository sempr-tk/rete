include_directories(${PROJECT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(Pegmatite REQUIRED pegmatite)
include_directories(${Pegmatite_INCLUDE_DIRS})
link_directories(${Pegmatite_LIBRARY_DIRS})

add_executable(AvoidDuplicate_241 AvoidDuplicate_241.cpp)
target_link_libraries(AvoidDuplicate_241 rete-core rete-rdf rete-reasoner)
add_test(NAME AvoidDuplicate_241 COMMAND AvoidDuplicate_241)

add_executable(NoDuplicateInAlpha NoDuplicateInAlpha.cpp)
target_link_libraries(NoDuplicateInAlpha rete-core rete-rdf rete-reasoner)
add_test(NAME NoDuplicateInAlpha COMMAND NoDuplicateInAlpha)

add_executable(RemoveCycle1 RemoveCycle1.cpp)
target_link_libraries(RemoveCycle1 rete-core rete-rdf rete-reasoner)
add_test(NAME RemoveCycle1 COMMAND RemoveCycle1)

add_executable(RemoveCycle2 RemoveCycle2.cpp)
target_link_libraries(RemoveCycle2 rete-core rete-rdf rete-reasoner)
add_test(NAME RemoveCycle2 COMMAND RemoveCycle2)

add_executable(RemoveCycle3 RemoveCycle3.cpp)
target_link_libraries(RemoveCycle3 rete-core rete-rdf rete-reasoner)
add_test(NAME RemoveCycle3 COMMAND RemoveCycle3)

add_executable(ParsePrefixes ParsePrefixes.cpp)
target_link_libraries(ParsePrefixes rete-core rete-rdf rete-reasoner)
add_test(NAME ParsePrefixes COMMAND ParsePrefixes)

add_executable(MathBuiltins MathBuiltins.cpp)
target_link_libraries(MathBuiltins rete-core rete-rdf rete-reasoner)
add_test(NAME MathBuiltins COMMAND MathBuiltins)

add_executable(NoValue NoValue.cpp)
target_link_libraries(NoValue rete-core rete-rdf rete-reasoner)
add_test(NAME NoValue COMMAND NoValue)

add_executable(NoValueComplex NoValueComplex.cpp)
target_link_libraries(NoValueComplex rete-core rete-rdf rete-reasoner)
add_test(NAME NoValueComplex COMMAND NoValueComplex)

add_executable(ParserExceptions ParserExceptions.cpp)
target_link_libraries(ParserExceptions rete-core rete-rdf rete-reasoner)
add_test(NAME ParserExceptions COMMAND ParserExceptions)

add_executable(MutableWMEs MutableWMEs.cpp)
target_link_libraries(MutableWMEs rete-core rete-rdf rete-reasoner)
add_test(NAME MutableWMEs COMMAND MutableWMEs)

add_executable(Grammar Grammar.cpp)
target_link_libraries(Grammar rete-core rete-rdf rete-reasoner)
add_test(NAME Grammar COMMAND Grammar)

add_executable(NetworkDeconstruction NetworkDeconstruction.cpp)
target_link_libraries(NetworkDeconstruction rete-core rete-rdf rete-reasoner)
add_test(NAME NetworkDeconstruction COMMAND NetworkDeconstruction)

add_executable(AddingRulesAfterData AddingRulesAfterData.cpp)
target_link_libraries(AddingRulesAfterData rete-core rete-rdf rete-reasoner)
add_test(NAME AddingRulesAfterData COMMAND AddingRulesAfterData)

add_executable(RemovingRules RemovingRules.cpp)
target_link_libraries(RemovingRules rete-core rete-rdf rete-reasoner)
add_test(NAME RemovingRules COMMAND RemovingRules)

add_executable(TrueTest TrueTest.cpp)
target_link_libraries(TrueTest rete-core rete-rdf rete-reasoner)
add_test(NAME TrueTest COMMAND TrueTest)

add_executable(GroupBy GroupBy.cpp)
target_link_libraries(GroupBy rete-core rete-rdf rete-reasoner)
add_test(NAME GroupBy COMMAND GroupBy)

add_executable(GroupByBuiltins GroupByBuiltins.cpp)
target_link_libraries(GroupByBuiltins rete-core rete-rdf rete-reasoner)
add_test(NAME GroupByBuiltins COMMAND GroupByBuiltins)

add_executable(GlobalConstants GlobalConstants.cpp)
target_link_libraries(GlobalConstants rete-core rete-rdf rete-reasoner)
add_test(NAME GlobalConstants COMMAND GlobalConstants)

add_executable(StringsInTriples StringsInTriples.cpp)
target_link_libraries(StringsInTriples rete-core rete-rdf rete-reasoner)
add_test(NAME StringsInTriples COMMAND StringsInTriples)

add_executable(MakeSkolem MakeSkolem.cpp)
target_link_libraries(MakeSkolem rete-core rete-rdf rete-reasoner)
add_test(NAME MakeSkolem COMMAND MakeSkolem)

add_executable(Comments Comments.cpp)
target_link_libraries(Comments rete-core rete-rdf rete-reasoner)
add_test(NAME Comments COMMAND Comments)

add_executable(Scopes Scopes.cpp)
target_link_libraries(Scopes rete-core rete-rdf rete-reasoner)
add_test(NAME Scopes COMMAND Scopes)

add_executable(SubRules SubRules.cpp)
target_link_libraries(SubRules rete-core rete-rdf rete-reasoner)
add_test(NAME SubRules COMMAND SubRules)

add_executable(Callbacks Callbacks.cpp)
target_link_libraries(Callbacks rete-core rete-rdf rete-reasoner)
add_test(NAME Callbacks COMMAND Callbacks)


add_executable(AnnotatedRules AnnotatedRules.cpp)
target_link_libraries(AnnotatedRules rete-core rete-rdf rete-reasoner)
add_test(NAME AnnotatedRules COMMAND AnnotatedRules)

add_executable(AnnotatedNoValue AnnotatedNoValue.cpp)
target_link_libraries(AnnotatedNoValue rete-core rete-rdf rete-reasoner)
add_test(NAME AnnotatedNoValue COMMAND AnnotatedNoValue)

add_executable(AnnotatedGroupBy AnnotatedGroupBy.cpp)
target_link_libraries(AnnotatedGroupBy rete-core rete-rdf rete-reasoner)
add_test(NAME AnnotatedGroupBy COMMAND AnnotatedGroupBy)

add_executable(ExplanationTest ExplanationTest.cpp)
target_link_libraries(ExplanationTest rete-core rete-rdf rete-reasoner)
add_test(NAME ExplanationTest COMMAND ExplanationTest)

add_executable(test_rete main.cpp)
target_link_libraries(test_rete rete-core rete-rdf rete-reasoner)
add_test(NAME main COMMAND test_rete)
