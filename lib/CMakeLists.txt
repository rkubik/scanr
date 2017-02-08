set(CMAKE_C_FLAGS "-ggdb")

include_directories( 
    . 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
)

set( LIBSOURCES 
    ${PROJECT_SOURCE_DIR}/src/scan.c
    ${PROJECT_SOURCE_DIR}/src/util/net.c
    ${PROJECT_SOURCE_DIR}/src/util/time.c
    ${PROJECT_SOURCE_DIR}/src/util/queue.c
    ${PROJECT_SOURCE_DIR}/src/util/workpool.c
)

add_library( libscanr_shared SHARED ${LIBSOURCES} )
add_library( libscanr_static STATIC ${LIBSOURCES} )

set_target_properties( libscanr_shared PROPERTIES
    COMPILE_FLAGS  "-fPIC -Wall -O2"
    VERSION "${PACKAGE_MINOR}.${PACKAGE_PATCH}"
    SOVERSION "${PACKAGE_MAJOR}"
    OUTPUT_NAME scanr
)

set_target_properties( libscanr_static PROPERTIES
    OUTPUT_NAME scanr
)

target_link_libraries( libscanr_shared -lpthread )
target_link_libraries( libscanr_static -lpthread )

install( TARGETS libscanr_shared LIBRARY DESTINATION usr/lib )
install( TARGETS libscanr_static ARCHIVE DESTINATION usr/lib )
