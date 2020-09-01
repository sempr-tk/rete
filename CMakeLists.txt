cmake_minimum_required(VERSION 3.5)

project(rete VERSION 0.6.0)
add_definitions(-Wall -Wextra -pedantic -Wnon-virtual-dtor -Werror=unused-result)
set(CMAKE_CXX_STANDARD 14)

option(DEBUG_PARSING "enable debug output while parsing rules" false)
if(${DEBUG_PARSING})
    add_definitions(-DDEBUG_PARSING)
endif()

option(DEBUG_AST "enable debug output while constructing AST" false)
if (${DEBUG_AST})
    add_definitions(-DDEBUG_AST_CONSTRUCTION)
endif()

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined")

add_subdirectory(rete-core)
add_subdirectory(rete-rdf)
add_subdirectory(rete-reasoner)
add_subdirectory(examples)

enable_testing()
add_subdirectory(test)


# configure for cmake install
set(RETE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
configure_file("rete.pc.in" "rete-cmake.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/rete-cmake.pc"
    DESTINATION lib/pkgconfig
    RENAME rete-dev.pc
    COMPONENT pkgconfig-cmake
)

# configure for debian package
set(RETE_INSTALL_PREFIX "/usr")
configure_file("rete.pc.in" "rete-pkgconfig.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/rete-pkgconfig.pc"
    DESTINATION lib/pkgconfig
    RENAME rete.pc
    COMPONENT pkgconfig-deb
)

# configure cpack
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "rete pattern matching and rule based reasoner")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Nils Niemann <nils.niemann@dfki.de>")
set(CPACK_COMPONENTS_ALL "core;rdf;reasoner;pkgconfig-deb") # excludes the rete-dev.pc
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) # use dpkg-shlibdeps to find dependencies
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON) # create shlibs control file automatically. Makes shlibdeps find this library as a dependency of others!
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY ">=")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpegmatite (>= 0.1)")

# always last:
include(CPack)

