# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(project_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

# set thirdparty_base_dir before include.
set(third_party_base_dir ${project_base_dir}/third_party)
include(${project_base_dir}/cmake/android_third_party.cmake)

# build native_app_glue as a static lib
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)

if (ENABLE_CLOUDXR STREQUAL "1")
	message(STATUS "=========start cloudxr ${CLOUDXR_LIBPATH}")
	# Import CloudXR
	add_library(cloudxr-lib SHARED IMPORTED)
	set_target_properties(cloudxr-lib PROPERTIES IMPORTED_LOCATION
			${CLOUDXR_LIBPATH}/${ANDROID_ABI}/libCloudXRClient.so
			)
	include_directories(${CLOUDXR_INCLUDE})

	add_definitions(-DENABLE_CLOUDXR)
endif()

add_definitions(-DXR_USE_PLATFORM_ANDROID)
add_definitions(-DXR_USE_GRAPHICS_API_OPENGL_ES)
# XR_USE_TIMESPEC
# add_definitions(-DXR_USE_TIMESPEC)
# add_definitions(-DXR_USE_GRAPHICS_API_VULKAN)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# source dir
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB LOCAL_HEADERS  "*.h")
file(GLOB LOCAL_SOURCE   "*.cpp")
file(GLOB VULKAN_SHADERS "vulkan_shaders/*.glsl")

LINK_DIRECTORIES(${third_party_base_dir}/openxr/lib/android/${ANDROID_ABI})
include_directories(${third_party_base_dir}/openxr/include)

add_library( # Sets the name of the library.
        lark_xr_openxr_pico
		SHARED
        # ${LOCAL_SOURCE}
        # ${LOCAL_HEADERS}
		# ${VULKAN_SHADERS}
		${third_party_base_dir}/openxr/include/common/gfxwrapper_opengl.c
		check.h
		common.h
		frame_buffer.h
		frame_buffer.cpp
		geometry.h
		graphics_device_android.h
		graphics_device_android.cpp
		logger.h
		logger.cpp
		main.cpp
		openxr_context.h
		openxr_context.cpp
		options.h
		pch.h
		pch.cpp
		platformdata.h
		platformplugin.h
		platformplugin_android.cpp
		platformplugin_factory.cpp
		pvr_xr_application.h
		pvr_xr_application.cpp
		pvr_xr_jni.cpp
		pvr_xr_scene.h
		pvr_xr_scene.cpp
		pvr_xr_scene_cloud.h
		pvr_xr_scene_cloud.cpp
		pvr_xr_scene_local.h
		pvr_xr_scene_local.cpp
		pvr_xr_utils.h
		pvr_xr_utils.cpp
		xr_input_state.h
		xr_input_state.cpp
)

source_group("Headers" FILES ${LOCAL_HEADERS})
source_group("Shaders" FILES ${VULKAN_SHADERS})


if(VulkanHeaders_INCLUDE_DIRS)
    target_include_directories(lark_xr_openxr_pico
            PRIVATE
            ${VulkanHeaders_INCLUDE_DIRS}
            )
endif()

if (BUILD_WIN)
	set(hpsocket_include "${third_party_base_dir}/hp-socket/include/windows/HPSocket")
else()
	set(hpsocket_include "${third_party_base_dir}/hp-socket/include/linux/hpsocket")
endif()


target_include_directories(lark_xr_openxr_pico
		PRIVATE
		# pxy gl
		${project_base_dir}/lib_pxygl/src/main/cpp
		# common ui
		${project_base_dir}/lib_xr_common_ui/src/main/cpp
		# lib xr
		${project_base_dir}/lark_xr/include
		${third_party_base_dir}/freetype/include/
		${third_party_base_dir}/glm/include/
		${third_party_base_dir}/assimp/include/
		${third_party_base_dir}/tinyobj/src/
		${third_party_base_dir}/oboe/include/
)

if(Vulkan_LIBRARY)
    target_link_libraries(lark_xr_openxr_pico ${Vulkan_LIBRARY})
endif()

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       lark_xr_openxr_pico
		               lib_openxr_loader
                       android
		               jnigraphics
                       vulkan
					   EGL
					   GLESv3
                       app-glue
                       log
						# first party
						lib_lark_common
						lib_lark_pxygl
						lib_lark_xr
						lib_lark_xr_common_ui
						# assimp android port
						lib_assimp_android_jniiosystem
						# ndk glue
						#        app_glue
						# oboe
						liboboe
)

if (ENABLE_CLOUDXR STREQUAL "1")
	target_link_libraries( # Specifies the target library.
			lark_xr_openxr_pico
			cloudxr-lib
	)
endif()