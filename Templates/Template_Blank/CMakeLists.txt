cmake_minimum_required(VERSION 3.10)

# This is the root project for your game.
project(#Template_Blank# VERSION 1.0)
set(EXE_BASE_NAME "#Template_Blank#")

# --------------------- C++ STANDARD & FLAGS (Applied globally) ---------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# =======================================================
# --------------------- Link Termynth Engine ---------------------
# =======================================================

option(USE_ENGINE_SUBMODULE "Use TermynthEngine as submodule" ON)

if(USE_ENGINE_SUBMODULE)
	# Engine is in same repo as submodule
	add_subdirectory(#Engine_Path#)
	set(ENGINE_CLIENT_LIB TermynthEngine)
    set(ENGINE_SERVER_LIB TermynthEngineServer)
else()
	# Engine is installed on system
	find_package(Termynth 1.0 REQUIRED)
	set(ENGINE_CLIENT_LIB Termynth::TermynthEngine)
    set(ENGINE_SERVER_LIB Termynth::TermynthEngineServer)
endif()

# =======================================================
# --------------------- Executable Targets ---------------------
# =======================================================

file(GLOB_RECURSE GAME_SOURCES "Source/Private/*.cpp")

# --- 1. Client Executable ---
add_executable(${EXE_BASE_NAME} ${GAME_SOURCES})
target_link_libraries(${EXE_BASE_NAME} PRIVATE ${ENGINE_CLIENT_LIB}) 
target_include_directories(${EXE_BASE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Source/Public)

# --- 2. Server Executable ---
add_executable(${EXE_BASE_NAME}Server ${GAME_SOURCES})
target_link_libraries(${EXE_BASE_NAME}Server PRIVATE ${ENGINE_SERVER_LIB})
target_compile_definitions(${EXE_BASE_NAME}Server PRIVATE DEDICATED_SERVER)
target_include_directories(${EXE_BASE_NAME}Server PRIVATE ${PROJECT_SOURCE_DIR}/Source/Public)


# Common settings for all executables
foreach(TARGET IN ITEMS ${EXE_BASE_NAME} ${EXE_BASE_NAME}Server)
    # Set output name (e.g., Termynth-Server)
    if("${TARGET}" STREQUAL "Server")
        set(base "${EXE_BASE_NAME}-Server")
    else()
        set(base "${EXE_BASE_NAME}")
    endif()
    
    set_target_properties(${TARGET} PROPERTIES
        OUTPUT_NAME "${base}$<$<CONFIG:Debug>:-Debug>$<$<CONFIG:Development>:-Development>"
    )
endforeach()

# =======================================================
# --------------------- Output and Assets ---------------------
# =======================================================

if(WIN32)
    set(PLATFORM_NAME "Win64")
elseif(UNIX)
    set(PLATFORM_NAME "Linux")
endif()

# Base output folder
set(BASE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/Binaries/${PLATFORM_NAME}")
set(INTERMEDIATE_DIR "${CMAKE_BINARY_DIR}/Intermediate/${PLATFORM_NAME}")

# Configure output directories for executables and libraries
foreach(TARGET IN ITEMS ${EXE_BASE_NAME} ${EXE_BASE_NAME}Server)
    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIR}/$<CONFIG>"
        OBJECT_OUTPUT_DIRECTORY "${INTERMEDIATE_DIR}/$<CONFIG>/${TARGET}"
    )
endforeach()

# --------------------- Copy Content ---------------------

set(CONTENT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Content")

if (Termynth_FOUND)
	get_filename_component(Termynth_INSTALL_PREFIX "${Termynth_DIR}/../.." ABSOLUTE)
	set(Termynth_CONTENT_DIR "${Termynth_INSTALL_PREFIX}/share/Termynth-${Termynth_VERSION}")
endif()

# Check if engine was found and has the content variable set
if(NOT USE_ENGINE_SUBMODULE AND Termynth_FOUND)
    # Define the engine content source path (from the installed package config)
    set(ENGINE_CONTENT_SOURCE "${Termynth_CONTENT_DIR}/Content")
else()
    set(ENGINE_CONTENT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/#Engine_Path#/Content")
endif()

message(STATUS "Termynth Content location: ${ENGINE_CONTENT_SOURCE}")

# Post-build command to copy Content folder
foreach(TARGET IN ITEMS #Template_Blank# #Template_Blank#Server)
    get_target_property(RUNTIME_DIR ${TARGET} RUNTIME_OUTPUT_DIRECTORY)

	# 1. Copy GAME Content
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CONTENT_SOURCE}"
        "${RUNTIME_DIR}/Content"
        COMMENT "Copying Game Content folder to ${RUNTIME_DIR}"
    )
	
	# 2. Copy ENGINE Content
	add_custom_command(TARGET ${TARGET} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${ENGINE_CONTENT_SOURCE}"
		"${RUNTIME_DIR}/Content/Engine"
		COMMENT "Copying Engine Content folder to ${RUNTIME_DIR}"
		)
endforeach()
