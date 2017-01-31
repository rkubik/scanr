set( CMAKE_C_FLAGS "-ggdb -Wl,-z,relro -Werror -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong -Wformat -Werror=format-security" )

include_directories(
    .
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}
)

set( SOURCES
    ${PROJECT_SOURCE_DIR}/src/scanr.c
)

add_executable( scanr ${SOURCES} )

target_link_libraries( scanr libscanr_static )

install( TARGETS scanr RUNTIME DESTINATION usr/sbin )
