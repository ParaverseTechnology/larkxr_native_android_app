# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
#
set(LIBNAME lib_pxygl)
set(LARK_LIBNAME "lark_pxygl")

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fexceptions -std=c++11")

set(project_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)
# set thirdparty_base_dir before include.
set(third_party_base_dir ${project_base_dir}/third_party/)
# use assimp to load model.
# if turn off use tinyobj loader instead.
set(ENABLE_ASSIMP ON)

if (ENABLE_ASSIMP)
ADD_DEFINITIONS(-DENABLE_ASSIMP)
endif()

set(STATIC_POCO_LIB OFF)
include(${project_base_dir}/cmake/android_third_party.cmake)

# source dir
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs/${LIBNAME}/lib)

# export dll
if (STATIC_LIB) 
    add_definitions(-DLARK_PXYGL_EXPORT)
else() 
    add_definitions(-DLARK_PXYGL_DLL -DLARK_PXYGL_EXPORT)
endif()

# source files
set(lark_pxygl_source_files 
    ${thirdparty_src_glad}
    ${src_dir}/pxygl.cpp
    ${src_dir}/logger.h
    ${src_dir}/vertex_array_object.cpp
    ${src_dir}/transform.cpp
    ${src_dir}/shader.cpp
    ${src_dir}/texture.cpp
    ${src_dir}/mesh.cpp
    ${src_dir}/model.cpp
    ${src_dir}/object.cpp
    ${src_dir}/asset_file.cpp
    ${src_dir}/bitmap_factory.cpp
    ${src_dir}/skybox.cpp
    ${src_dir}/asset_loader.cpp
)

if (ENABLE_ASSIMP)
else()
    list(APPEND lark_pxygl_source_files ${thirdparty_src_tinyobj})
endif()

# header files
set(lark_pxygl_header_files
    ${src_dir}/pxygl.h
    ${src_dir}/logger.cpp
    ${src_dir}/vertex_array_object.h
    ${src_dir}/transform.h
    ${src_dir}/shader.h
    ${src_dir}/texture.h
    ${src_dir}/mesh.h
    ${src_dir}/model.h
    ${src_dir}/object.h
    ${src_dir}/skybox.h
    ${src_dir}/asset_loader.h
)

add_definitions(-D_GLM_ENABLE_EXPERIMENTAL)

# 生成链接库
if (STATIC_LIB)
    add_library(${LIBNAME} STATIC 
        ${lark_pxygl_source_files}
        ${lark_pxygl_header_files}
    )
else()
    add_library(${LIBNAME} SHARED 
        ${lark_pxygl_source_files}
        ${lark_pxygl_header_files}
    )
endif()


add_library(${LARK_LIBNAME} ALIAS ${LIBNAME})

set_target_properties(${LIBNAME}
    PROPERTIES
    OUTPUT_NAME "${LARK_LIBNAME}"
)

# include
target_include_directories(${LIBNAME} PRIVATE
    ${src_dir}
    # stb
    ${third_party_base_dir}/stb/include/
    # glm
    ${third_party_base_dir}/glm/include/
)

if (ENABLE_ASSIMP)
target_include_directories(${LIBNAME} PRIVATE
    ${third_party_base_dir}/assimp/include/
)
else()
target_include_directories(${LIBNAME} PRIVATE
    ${thirdparty_include_tinyobj}
)
endif()

find_library( # Sets the name of the path variable.
        log-lib
        android
        EGL
        GLESv3
        OpenSLES
        jnigraphics
)
set(EXT_LIBS
    android
    EGL
    GLESv3
    OpenSLES
    jnigraphics
    log
    # glm
    lib_glm
)

if (ENABLE_ASSIMP)
    list(APPEND EXT_LIBS lib_assimp)
    list(APPEND EXT_LIBS lib_assimp_android_jniiosystem)
endif()

include(${project_base_dir}/cmake/utils.cmake)
ANDROID_SHARE_OUTPUT(${LIBNAME} ${project_base_dir})

target_link_libraries(
    ${LIBNAME}
    ${EXT_LIBS}
)
