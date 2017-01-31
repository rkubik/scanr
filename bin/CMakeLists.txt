set( CMAKE_C_FLAGS "-ggdb -Wl,-z,relro -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong -Wformat -Werror=format-security" )

include_directories(
    .
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
)

set( SOURCES
    ${PROJECT_SOURCE_DIR}/src/scanr.c
)

add_executable( scanr ${SOURCES} )

install( TARGETS scanr RUNTIME DESTINATION usr/sbin )
