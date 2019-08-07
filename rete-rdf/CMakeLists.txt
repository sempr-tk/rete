set(RDF_SRC
    InferTriple.cpp
    Triple.cpp
    TripleAccessor.cpp
    TripleAlpha.cpp
    TripleConsistency.cpp
)

include_directories(${PROJECT_SOURCE_DIR})

add_library(rete-rdf SHARED ${RDF_SRC})
target_link_libraries(rete-rdf rete-core)
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-rdf
    LIBRARY DESTINATION lib
)
