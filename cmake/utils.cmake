

# find framework in mac os.
macro(ADD_FRAMEWORK fwname appname)
find_library(FRAMEWORK_${fwname}
NAMES ${fwname}
PATHS ${CMAKE_OSX_SYSROOT}/System/Library
PATH_SUFFIXES Frameworks
NO_DEFAULT_PATH)
if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
    MESSAGE(ERROR ": Framework ${fwname} not found")
else()
    TARGET_LINK_LIBRARIES(${appname} "${FRAMEWORK_${fwname}}")
    MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
endif()
endmacro(ADD_FRAMEWORK)

# android static output path setup
# setup 
macro(ANDROID_STATIC_OUTPUT libname projectBase)
	set_target_properties(${libname}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY # static lib output property
		${projectBase}/first_party/${libname}/lib/android/${ANDROID_ABI}
	)
endmacro(ANDROID_STATIC_OUTPUT)

# android share output path setup
# setup 
macro(ANDROID_SHARE_OUTPUT libname projectBase)
	set_target_properties(${libname}
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY # static lib output property
		${projectBase}/first_party/${libname}/lib/android/${ANDROID_ABI}
	)
endmacro(ANDROID_SHARE_OUTPUT)

# android first party shared setup
macro(ANDROID_FIRST_PARTH_SHARED projectBase name path lib)
    # setup path
    set(first_party_base_dir ${projectBase}/first_party)
    set(lib_platform_lib /lib)
    set(lib_platform_lib_prex /${lib_platform_lib}/android/)
    set(lib_android_prex /${lib_platform_lib_prex}${CMAKE_ANDROID_ARCH_ABI}/)
    # setup lib
    add_library(${name} SHARED IMPORTED)
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION
            ${first_party_base_dir}/${path}/${lib_platform_lib_prex}/${CMAKE_ANDROID_ARCH_ABI}/${lib}.so)
endmacro(ANDROID_FIRST_PARTH_SHARED)

# android first party static setup
macro(ANDROID_FIRST_PARTH_STATIC projectBase name path lib)
    # setup path
    set(first_party_base_dir ${projectBase}/first_party)
    set(lib_platform_lib /lib)
    set(lib_platform_lib_prex /${lib_platform_lib}/android/)
    set(lib_android_prex /${lib_platform_lib_prex}${CMAKE_ANDROID_ARCH_ABI}/)
    # setup lib
    add_library(${name} STATIC IMPORTED)
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION
            ${first_party_base_dir}/${path}/${lib_platform_lib_prex}/${CMAKE_ANDROID_ARCH_ABI}/${lib}.a)
endmacro(ANDROID_FIRST_PARTH_STATIC)

# android third party setup
macro(ANDROID_THIRD_PARTY_SHARED projectBase name path lib)
    # setup path
    set(third_party_base_dir ${projectBase}/third_party)
    set(lib_platform_lib /lib)
    set(lib_platform_lib_prex /${lib_platform_lib}/android/)
    set(lib_android_prex /${lib_platform_lib_prex}${CMAKE_ANDROID_ARCH_ABI}/)
    # setup lib
    add_library(${name} SHARED IMPORTED)
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION
            ${third_party_base_dir}/${path}/${lib_platform_lib_prex}/${CMAKE_ANDROID_ARCH_ABI}/${lib}.so)
endmacro(ANDROID_THIRD_PARTY_SHARED)

# android third party setup
macro(ANDROID_THIRD_PARTY_STATIC projectBase name path lib)
    # setup path
    set(third_party_base_dir ${projectBase}/third_party)
    set(lib_platform_lib /lib)
    set(lib_platform_lib_prex /${lib_platform_lib}/android/)
    set(lib_android_prex /${lib_platform_lib_prex}${CMAKE_ANDROID_ARCH_ABI}/)
    # setup lib
    add_library(${name} STATIC IMPORTED)
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION
            ${third_party_base_dir}/${path}/${lib_platform_lib_prex}/${CMAKE_ANDROID_ARCH_ABI}/${lib}.a)
endmacro(ANDROID_THIRD_PARTY_STATIC)