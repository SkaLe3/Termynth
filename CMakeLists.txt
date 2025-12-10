cmake_minimum_required(VERSION 3.10)

set(ENGINE_VERSION 1.0)

project(TermynthEngine VERSION ${ENGINE_VERSION})
set(LIB_BASE_NAME "TermynthRuntime")


# --------------------- C++ STANDARD & FLAGS ---------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Development;Shipping" CACHE STRING "" FORCE)

if(MSVC)
    # MSVC warning levels and defines
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /Od /DDEBUG")
set(CMAKE_CXX_FLAGS_DEVELOPMENT "${CMAKE_CXX_FLAGS_DEVELOPMENT} /W4 /O1 /DDEVELOPMENT")
set(CMAKE_CXX_FLAGS_SHIPPING "${CMAKE_CXX_FLAGS_SHIPPING} /W4 /O2 /DNDEBUG /DSHIPPING")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE STRING "Linker flags for DEBUG executables")
	set(CMAKE_EXE_LINKER_FLAGS_DEVELOPMENT "" CACHE STRING "Linker flags for DEVELOPMENT executables")
	set(CMAKE_EXE_LINKER_FLAGS_SHIPPING "" CACHE STRING "Linker flags for SHIPPING executables")
	# Always use dynamic CRT for all configs
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # Fix iterator debug level
    add_compile_options(
        $<$<CONFIG:Debug>:/D_ITERATOR_DEBUG_LEVEL=2>
        $<$<CONFIG:Development>:/D_ITERATOR_DEBUG_LEVEL=0>
		$<$<CONFIG:Shipping>:/D_ITERATOR_DEBUG_LEVEL=0>
    )
else()
    # GCC/Clang flags
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEVELOPMENT "-g -O1 -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_SHIPPING "-O2 -DNDEBUG")
endif()

# =======================================================
# --------------------- Source File Definition ---------------------
# =======================================================

# 1. Core/Common/Server-side Source Files (REQUIRED BY BOTH LIB TARGETS)
file(GLOB_RECURSE COMMON_AND_SERVER_SOURCES
    "Source/Runtime/Termynth/Private/Core/*.cpp"
    "Source/Runtime/Termynth/Private/Engine/*.cpp"
	"Source/Runtime/Termynth/Private/Platform/*.cpp"
    "Source/Runtime/Termynth/Private/Networking/*.cpp"
    "Source/Runtime/Termynth/Private/Server/*.cpp"
	"Source/Runtime/Termynth/Private/Utils/*.cpp"
)

# 2. Client-only Source Files (ONLY FOR FULL ENGINE LIB)
file(GLOB_RECURSE CLIENT_ONLY_SOURCES
    "Source/Runtime/Termynth/Private/Render/*.cpp"
    "Source/Runtime/Termynth/Private/Audio/*.cpp"
    "Source/Runtime/Termynth/Private/Input/*.cpp"
)

file(GLOB_RECURSE LAUNCH_MODULE_SOURCES
	"Source/Runtime/Launch/Private/*.cpp"
)

# 3. Final lists
list(APPEND ENGINE_SOURCES ${COMMON_AND_SERVER_SOURCES} ${CLIENT_ONLY_SOURCES})
set(SERVER_LIB_SOURCES ${COMMON_AND_SERVER_SOURCES})

# Remove platform-specific files not for this OS
if(WIN32)
    list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/Linux/.*")
    list(FILTER SERVER_LIB_SOURCES EXCLUDE REGEX ".*/Linux/.*")
elseif(UNIX)
    list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/Windows/.*")
    list(FILTER SERVER_LIB_SOURCES EXCLUDE REGEX ".*/Windows/.*")
endif()


# =======================================================
# --------------------- Engine Library Targets ---------------------
# =======================================================
include(GNUInstallDirs)
#-------------------------------------------------------------------

# --- 1. FULL Engine Library (For Client) ---
add_library(TermynthRuntime STATIC ${ENGINE_SOURCES})
# Setting PUBLIC includes ensures the root project can find engine headers
target_include_directories(TermynthRuntime PUBLIC 
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Runtime/Termynth/Public>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}>)

# --- 2. SERVER-ONLY Library (For Server) ---
add_library(TermynthRuntimeServer STATIC ${SERVER_LIB_SOURCES})
target_include_directories(TermynthRuntimeServer PUBLIC 
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Runtime/Termynth/Public>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}>)
			
target_compile_definitions(TermynthRuntimeServer PRIVATE DEDICATED_SERVER)


# --- 3. Launch Module
add_library(TermynthEngine STATIC ${LAUNCH_MODULE_SOURCES})
target_include_directories(TermynthEngine PUBLIC 
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Runtime/Termynth/Public>
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Runtime/Launch/Public>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}>)
			
add_library(TermynthEngineServer STATIC ${LAUNCH_MODULE_SOURCES})
target_include_directories(TermynthEngineServer PUBLIC 
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Runtime/Termynth/Public>
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Runtime/Launch/Public>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}>)

# =======================================================
# --------------------- Pak Builder Targets ---------------------
# =======================================================
file(GLOB_RECURSE PAK_BUILDER_SOURCES
    "Source/Programs/TermynthPakBuilder/Private/*.cpp"
)

add_executable(TermynthPakBuilder ${PAK_BUILDER_SOURCES})
target_include_directories(TermynthPakBuilder PUBLIC
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Runtime/Termynth/Public>
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Programs/TermynthPakBuilder/Public>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}>)
			

set_target_properties(TermynthPakBuilder PROPERTIES
	OUTPUT_NAME "TermynthPakBuilder$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
	)


# --------------------- Engine Library Linking ---------------------
foreach(LIB_TARGET IN ITEMS TermynthRuntime TermynthRuntimeServer)
    if(WIN32)
        target_link_libraries(${LIB_TARGET} PRIVATE ws2_32)
    elseif(UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(${LIB_TARGET} PRIVATE Threads::Threads)
    endif()
endforeach()

add_subdirectory(${PROJECT_SOURCE_DIR}/ThirdParty/zlib)
set(ZLIB_LIB zlibstatic CACHE INTERNAL "")

target_link_libraries(TermynthRuntime PRIVATE ${ZLIB_LIB})
target_link_libraries(TermynthRuntimeServer PRIVATE ${ZLIB_LIB})

set_target_properties(TermynthRuntime PROPERTIES
	OUTPUT_NAME "${LIB_BASE_NAME}$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
	)

set_target_properties(TermynthRuntimeServer PROPERTIES
	OUTPUT_NAME "${LIB_BASE_NAME}-Server$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
	)

target_link_libraries(TermynthEngine PRIVATE TermynthRuntime)
target_link_libraries(TermynthEngineServer PRIVATE TermynthRuntimeServer)
target_link_libraries(TermynthPakBuilder PRIVATE TermynthRuntime)

set_target_properties(TermynthEngine PROPERTIES
	OUTPUT_NAME "TermynthEngine$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
	)

set_target_properties(TermynthEngineServer PROPERTIES
	OUTPUT_NAME "TermynthEngine-Server$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
	)


# =======================================================
# --------------------- Output Configuration ---------------------
# =======================================================

if(WIN32)
    set(PLATFORM_NAME "Win64")
elseif(UNIX)
    set(PLATFORM_NAME "Linux")
endif()

# Base output folder for all binaries (libraries)
set(BASE_OUTPUT_DIR "${PROJECT_BINARY_DIR}/Binaries/${PLATFORM_NAME}")
set(INTERMEDIATE_DIR "${PROJECT_BINARY_DIR}/Intermediate/${PLATFORM_NAME}")

# Configure output directories for libraries and intermediate files.
foreach(TARGET IN ITEMS TermynthRuntime TermynthRuntimeServer TermynthEngine TermynthEngineServer)
    set_target_properties(${TARGET} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        OBJECT_OUTPUT_DIRECTORY "${INTERMEDIATE_DIR}/$<CONFIG>/${TARGET}"
    )
endforeach()

    set_target_properties(TermynthPakBuilder PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        OBJECT_OUTPUT_DIRECTORY "${INTERMEDIATE_DIR}/$<CONFIG>/TermynthPakBuilder"
    )

# Set the source content folder
set(CONTENT_SOURCE "${PROJECT_SOURCE_DIR}/Content")

# For each target
foreach(TARGET IN ITEMS TermynthRuntime TermynthRuntimeServer)
    get_target_property(RUNTIME_DIR ${TARGET} RUNTIME_OUTPUT_DIRECTORY)

    # Post-build command to copy Content folder
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CONTENT_SOURCE}"
        "${RUNTIME_DIR}/Content"
        COMMENT "Copying Content folder to ${RUNTIME_DIR}"
    )
endforeach()


# =======================================================
# --------------------- Installation ---------------------
# =======================================================


# Install libraries
install(TARGETS TermynthRuntime TermynthRuntimeServer TermynthEngine TermynthEngineServer ${ZLIB_LIB}
	EXPORT TermynthTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install public headers
install(DIRECTORY Source/Runtime/Termynth/Public/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}
)
install(DIRECTORY Source/Runtime/Launch/Public/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}
)
install(DIRECTORY Source/Programs/TermynthPakBuilder/Public/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}
)

# Install Content folder
install(DIRECTORY Content/ 
	DESTINATION ${CMAKE_INSTALL_DATADIR}/Termynth-${ENGINE_VERSION}/Content
)


# set(TERMYNTH_CONTENT_DATADIR "${PROJECT_SOURCE_DIR}/${CMAKE_INSTALL_PREFIX}/${TERMYNTH_DATADIR}/Termynth-${ENGINE_VERSION}")

# Generate CMake config for find_package
install(EXPORT TermynthTargets
	FILE TermynthTargets.cmake
	NAMESPACE Termynth::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/Termynth-${ENGINE_VERSION}
)

include(CMakePackageConfigHelpers)

# Configure the main package file. 
# This automatically includes logic to find the install prefix relative to itself
# and includes the generated TermynthTargets.cmake file.
configure_package_config_file(
    TermynthConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/TermynthConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/Termynth-${ENGINE_VERSION}"
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR # Used to set Termynth_INCLUDE_DIRS
	CMAKE_INSTALL_DATADIR 
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/TermynthConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Termynth-${ENGINE_VERSION}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/TermynthConfigVersion.cmake"
    VERSION "${ENGINE_VERSION}"
    COMPATIBILITY ExactVersion 
    # Use AnyNewerVersion if minor/patch updates are guaranteed to be compatible.
    # Use ExactVersion if compatibility isn't guaranteed.
)

install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/TermynthConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Termynth-${ENGINE_VERSION}
)

# Also provide a package config for build-tree usage (submodule)
export(EXPORT TermynthTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/TermynthTargets.cmake"
	NAMESPACE Termynth::
)


