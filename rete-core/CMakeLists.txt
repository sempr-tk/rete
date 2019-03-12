set(CORE_SRC
    Accessors.cpp
    Agenda.cpp
    AgendaNode.cpp
    AlphaBetaAdapter.cpp
    AlphaMemory.cpp
    AlphaNode.cpp
    BetaComparator.cpp
    BetaMemory.cpp
    BetaNode.cpp
    Builtin.cpp
    JoinNode.cpp
    JoinUnconditional.cpp
    Network.cpp
    Node.cpp
    Production.cpp
    ProductionNode.cpp
    Token.cpp
    Util.cpp
    WME.cpp
    WMEComparator.cpp
)

set(BUILTIN_SRC
    builtins/Math.cpp
    builtins/Util.cpp
)

add_library(rete-core SHARED ${CORE_SRC} ${BUILTIN_SRC})
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-core
    LIBRARY DESTINATION lib
)
