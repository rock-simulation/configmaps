cmake_minimum_required(VERSION 2.6)

project(configmaps)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "This library contains a C++ implementation of python dictionary-like containers - ConfigMaps.")

include(FindPkgConfig)

pkg_check_modules(YAML REQUIRED
        yaml-cpp
        jsoncpp
)
include_directories(${YAML_INCLUDE_DIRS})
link_directories(${YAML_LIBRARY_DIRS})
add_definitions(${YAML_CFLAGS_OTHER})  #flags excluding the ones with -I

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-DCOMPILE_UNIT_TESTS)

# check the path to json header
include(CheckIncludeFileCXX)
check_include_file_cxx(jsoncpp/json/json.h HAVE_JSONCPP_JSON_H)
if (NOT HAVE_JSONCPP_JSON_H)
  check_include_file_cxx (json/json.h HAVE_JSON_H)
endif(NOT HAVE_JSONCPP_JSON_H)

# cmake variables
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)

set(SOURCES
    src/ConfigMap.cpp
    src/ConfigItem.cpp
    src/ConfigVector.cpp
    src/ConfigBase.cpp
)
set(HEADERS
    src/ConfigData.h
    src/ConfigMap.hpp
    src/ConfigVector.hpp
    src/ConfigItem.hpp
    src/ConfigAtom.hpp
    src/ConfigBase.hpp
    src/FIFOMap.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(
        ${PROJECT_NAME}
        ${YAML_LIBRARIES}
        -lpthread
)

if(WIN32)
  set(LIB_INSTALL_DIR bin) # .dll are in PATH, like executables
else(WIN32)
  set(LIB_INSTALL_DIR lib)
endif(WIN32)


set(_INSTALL_DESTINATIONS
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION lib
)


# Install the library into the lib folder
install(TARGETS ${PROJECT_NAME} ${_INSTALL_DESTINATIONS})

# Install headers into mars include directory
install(FILES ${HEADERS} DESTINATION include/configmaps)
install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}")

# Prepare and install necessary files to support finding of the library
# using pkg-config
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)




