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
    BetaBetaNode.cpp
    Builtin.cpp
    connect.cpp
    GroupBy.cpp
    JoinNode.cpp
    Network.cpp
    Node.cpp
    NoValue.cpp
    Production.cpp
    ProductionNode.cpp
    Token.cpp
    TokenGroup.cpp
    TokenGroupAccessor.cpp
    TrueNode.cpp
    Util.cpp
    WME.cpp
    WMEComparator.cpp
)

set(BUILTIN_SRC
    builtins/Math.cpp
    builtins/Util.cpp
)

add_library(rete-core SHARED ${CORE_SRC} ${BUILTIN_SRC})
set_target_properties(rete-core PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    COMPONENT core
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS rete-core
    LIBRARY DESTINATION lib COMPONENT core
)
