set(CORE_SRC
    Agenda.cpp
    AgendaNode.cpp
    AlphaBetaAdapter.cpp
    AlphaMemory.cpp
    AlphaNode.cpp
    BetaComparator.cpp
    BetaMemory.cpp
    BetaNode.cpp
    JoinNode.cpp
    JoinUnconditional.cpp
    Network.cpp
    Node.cpp
    Production.cpp
    ProductionNode.cpp
    Token.cpp
    Util.cpp
    ValueAccessor.cpp
    WME.cpp
    WMEComparator.cpp
)

add_library(rete-core SHARED ${CORE_SRC})
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-core
    LIBRARY DESTINATION lib
)
