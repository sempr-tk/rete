cmake_minimum_required(VERSION 2.8)

project(rete)
add_definitions(-std=c++11 -Wall)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

add_subdirectory(rete-core)
add_subdirectory(rete-rdf)
add_subdirectory(rete-reasoner)

enable_testing()
add_subdirectory(test)


configure_file("rete.pc.in" "rete.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/rete.pc"
    DESTINATION lib/pkgconfig
)
