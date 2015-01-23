cmake_minimum_required(VERSION 2.6)

project(configmaps)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "This library contains a C++ implementation of python dictionary-like containers - ConfigMaps.")

include(FindPkgConfig)
include(${CMAKE_INSTALL_PREFIX}/cmake/mars.cmake)

mars_defaults()
define_module_info()

pkg_check_modules(PKGCONFIG REQUIRED
        yaml-cpp
)
include_directories(${PKGCONFIG_INCLUDE_DIRS})
link_directories(${PKGCONFIG_LIBRARY_DIRS})
add_definitions(${PKGCONFIG_CFLAGS_OTHER})  #flags excluding the ones with -I

set(SOURCES 
    src/ConfigData.cpp
)
set(HEADERS
    src/ConfigData.h
    src/FIFOMap.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(
        ${PROJECT_NAME}
        ${PKGCONFIG_LIBRARIES} 
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

# Prepare and install necessary files to support finding of the library 
# using pkg-config
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)




