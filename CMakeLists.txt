cmake_minimum_required(VERSION 2.6.3)

project(BroPluginPF_RING)
include(BroPlugin)
find_package(PF_RING)

if ( PF_RING_FOUND )
    include_directories(BEFORE ${PF_RING_INCLUDE_DIR})
    bro_plugin_begin(Bro PF_RING)
    bro_plugin_cc(src/Plugin.cc)
    bro_plugin_cc(src/PF_RING.cc)
    # bro_plugin_bif(src/pf_ring.bif)
    bro_plugin_link_library(${PF_RING_LIBRARY})
    bro_plugin_dist_files(README.md VERSION)
    bro_plugin_end()

    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION LIMIT_COUNT 1)

    if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
        # Allows building rpm/deb packages via "make package" in build dir.
        include(ConfigurePackaging)
        ConfigurePackaging(${VERSION})
    endif ()

    message(STATUS "PF_RING prefix      : ${PF_RING_ROOT_DIR}")
else ()
    message(FATAL_ERROR "PF_RING library and/or headers not found.")
endif ()
