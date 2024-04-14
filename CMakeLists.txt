cmake_minimum_required( VERSION 3.19 )

project( vexflowcpp )

add_executable(${PROJECT_NAME})

target_compile_features( ${PROJECT_NAME}
    PRIVATE
        cxx_std_20
)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    if ( NOT APPLE )
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-missing-field-initializers)
    endif()
endif()

set( BUILD_OUTPUT_DIR "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/" )

set( INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/" )

target_include_directories( ${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external/quickjs" )
target_include_directories( ${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external/stb" )

add_library( quickjs
    OBJECT
        external/quickjs/cutils.c
        external/quickjs/libbf.c
        external/quickjs/libregexp.c
        external/quickjs/libunicode.c
        external/quickjs/quickjs.c
        external/quickjs/quickjs-libc.c
)
target_compile_features( quickjs
    PRIVATE
        c_std_99
)
target_compile_definitions( quickjs
    PUBLIC
        _GNU_SOURCE
        CONFIG_VERSION="2024-02-14"
        USE_BF_DEC
)
target_link_libraries( ${PROJECT_NAME} PUBLIC quickjs )

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(PYTHON3 python)
set(BUNDLED_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bravura.otf
    ${CMAKE_CURRENT_SOURCE_DIR}/external/vexflow-debug.js.js
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vexflow_wrap.js
)
add_custom_command(
   OUTPUT ${GENERATED_DIR}/bundle.h
   COMMAND ${PYTHON3} ${CMAKE_CURRENT_SOURCE_DIR}/tools/bundle_files.py --out ${GENERATED_DIR}/bundle.h ${BUNDLED_FILES}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/bundle_files.py ${BUNDLED_FILES}
)

add_subdirectory( src )
