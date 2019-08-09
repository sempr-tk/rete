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

add_executable(ParserExceptions ParserExceptions.cpp)
target_link_libraries(ParserExceptions rete-core rete-rdf rete-reasoner)
add_test(NAME ParserExceptions COMMAND ParserExceptions)

add_executable(MutableWMEs MutableWMEs.cpp)
target_link_libraries(MutableWMEs rete-core rete-rdf rete-reasoner)
add_test(NAME MutableWMEs COMMAND MutableWMEs)

add_executable(Grammar Grammar.cpp)
target_link_libraries(Grammar rete-core rete-rdf rete-reasoner)
add_test(NAME Grammar COMMAND Grammar)

add_executable(test_rete main.cpp)
target_link_libraries(test_rete rete-core rete-rdf rete-reasoner)
add_test(NAME main COMMAND test_rete)
