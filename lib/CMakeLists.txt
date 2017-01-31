include_directories( 
    . 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
)

set( LIBSOURCES 
    ${PROJECT_SOURCE_DIR}/src/scan.c
)

add_library( libscanr SHARED ${LIBSOURCES} )

set_target_properties( libscanr PROPERTIES
  COMPILE_FLAGS  "-fPIC -Wall -O2"
  VERSION "${PACKAGE_MINOR}.${PACKAGE_PATCH}"
  SOVERSION "${PACKAGE_MAJOR}"
  OUTPUT_NAME scanr )

target_link_libraries( libscanr )

install( TARGETS libscanr LIBRARY DESTINATION usr/lib )
