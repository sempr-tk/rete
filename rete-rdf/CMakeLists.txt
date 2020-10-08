set(RDF_SRC
    InferTriple.cpp
    MakeSkolem.cpp
    Skolem.cpp
    SkolemAccessor.cpp
    Triple.cpp
    TriplePart.cpp
    TripleAccessor.cpp
    TripleAlpha.cpp
    TripleTypeAlpha.cpp
    TripleConsistency.cpp
)

include_directories(${PROJECT_SOURCE_DIR})

add_library(rete-rdf SHARED ${RDF_SRC})
set_target_properties(rete-rdf PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

target_link_libraries(rete-rdf rete-core)
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    COMPONENT rdf
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-rdf
    LIBRARY DESTINATION lib COMPONENT rdf
)
