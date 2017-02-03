cmake_minimum_required( VERSION 2.8 )

project( scanr C)

set( CMAKE_INSTALL_PREFIX "/" )

set( PACKAGE_MAJOR 0 )
set( PACKAGE_MINOR 1 )
set( PACKAGE_PATCH 0 )

message( "MAJOR: ${PACKAGE_MAJOR}" )
message( "MINOR: ${PACKAGE_MINOR}" )
message( "PATCH: ${PACKAGE_PATCH}" )

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

add_subdirectory( include )
add_subdirectory( lib )
add_subdirectory( bin )
