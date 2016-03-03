cmake_minimum_required(VERSION 2.6)

project(configmaps)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "This library contains a C++ implementation of python dictionary-like containers - ConfigMaps.")

include(FindPkgConfig)

# is installed with https://github.com/rock-core/base-cmake.git
find_package(Rock)
rock_find_cmake(Boost)

pkg_check_modules(YAML REQUIRED
        yaml-cpp
)
include_directories(${YAML_INCLUDE_DIRS})
link_directories(${YAML_LIBRARY_DIRS})
add_definitions(${YAML_CFLAGS_OTHER})  #flags excluding the ones with -I

if(${YAML_VERSION} EQUAL "0.3.0")
  add_definitions(-DYAML_03_API)
  message("       WARNING: The use of yaml 0.3 is deprecated!")
endif()

set(SOURCES 
    src/ConfigData.cpp
)
set(HEADERS
    src/ConfigData.hpp
    src/FIFOMap.hpp
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

# MZ: some compatiblity headers after renaming. keep around for a month, then
# delete.
install(FILES src/ConfigData.h DESTINATION include/configmaps)
install(FILES src/FIFOMap.h DESTINATION include/configmaps)

# Prepare and install necessary files to support finding of the library 
# using pkg-config
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)




