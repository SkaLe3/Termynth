cmake_minimum_required(VERSION 3.10)

set(ENGINE_VERSION 1.0)

project(TermynthEngine VERSION ${ENGINE_VERSION})
set(LIB_BASE_NAME "TermynthEngine")


# --------------------- C++ STANDARD & FLAGS ---------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Development;Shipping" CACHE STRING "" FORCE)

# Default flags and MSVC definitions for consistency
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEVELOPMENT "-g -O1 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_SHIPPING "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if(MSVC)
    set(CMAKE_C_FLAGS_DEVELOPMENT "${CMAKE_C_FLAGS_RELEASE} /DDEVELOPMENT")
    set(CMAKE_CXX_FLAGS_DEVELOPMENT "${CMAKE_CXX_FLAGS_RELEASE} /DDEVELOPMENT")
    set(CMAKE_C_FLAGS_SHIPPING "${CMAKE_C_FLAGS_RELEASE} /DSHIPPING")
    set(CMAKE_CXX_FLAGS_SHIPPING "${CMAKE_CXX_FLAGS_RELEASE} /DSHIPPING")
endif()


# =======================================================
# --------------------- Source File Definition ---------------------
# =======================================================

# 1. Core/Common/Server-side Source Files (REQUIRED BY BOTH LIB TARGETS)
file(GLOB_RECURSE COMMON_AND_SERVER_SOURCES
    "Source/Private/Core/*.cpp"
    "Source/Private/Engine/*.cpp"
	"Source/Private/Platform/*.cpp"
    "Source/Private/Networking/*.cpp"
    "Source/Private/Server/*.cpp"
	"Source/Private/Launch.cpp"
)

# 2. Client-only Source Files (ONLY FOR FULL ENGINE LIB)
file(GLOB_RECURSE CLIENT_ONLY_SOURCES
    "Source/Private/Render/*.cpp"
    "Source/Private/Audio/*.cpp"
    "Source/Private/Input/*.cpp"
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
add_library(TermynthEngine STATIC ${ENGINE_SOURCES})
# Setting PUBLIC includes ensures the root project can find engine headers
target_include_directories(TermynthEngine PUBLIC 
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Public>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}>)

# --- 2. SERVER-ONLY Library (For Server) ---
add_library(TermynthEngineServer STATIC ${SERVER_LIB_SOURCES})
target_include_directories(TermynthEngineServer PUBLIC 
			$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Source/Public>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Termynth-${ENGINE_VERSION}>)
			
target_compile_definitions(TermynthEngineServer PRIVATE DEDICATED_SERVER)

# --------------------- Engine Library Linking ---------------------
foreach(LIB_TARGET IN ITEMS TermynthEngine TermynthEngineServer)
    if(WIN32)
        target_link_libraries(${LIB_TARGET} PRIVATE ws2_32)
    elseif(UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(${LIB_TARGET} PRIVATE Threads::Threads)
    endif()
endforeach()

set_target_properties(TermynthEngine PROPERTIES
	OUTPUT_NAME "${LIB_BASE_NAME}$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
	)

set_target_properties(TermynthEngineServer PROPERTIES
	OUTPUT_NAME "${LIB_BASE_NAME}-Server$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
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
foreach(TARGET IN ITEMS TermynthEngine TermynthEngineServer)
    set_target_properties(${TARGET} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        OBJECT_OUTPUT_DIRECTORY "${INTERMEDIATE_DIR}/$<CONFIG>/${TARGET}"
    )
endforeach()

# Set the source content folder
set(CONTENT_SOURCE "${PROJECT_SOURCE_DIR}/Content")

# For each target
foreach(TARGET IN ITEMS TermynthEngine TermynthEngineServer)
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
install(TARGETS TermynthEngine TermynthEngineServer
	EXPORT TermynthTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install public headers
install(DIRECTORY Source/Public/
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


