cmake_minimum_required(VERSION 3.5)
project(configmaps VERSION "0.1.0" DESCRIPTION "This library contains a C++ implementation of python dictionary-like containers - ConfigMaps.")

find_package(yaml-cpp REQUIRED)
find_package(jsoncpp REQUIRED)

link_directories(configmaps ${YAML_CPP_LIBRARY_DIR})

# Export the library interface
install(EXPORT configmaps-targets
	NAMESPACE configmaps::
	DESTINATION lib/cmake/configmaps
)
# Create and install the version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file("configmaps-config-version.cmake"
	VERSION ${configmaps_VERSION}
	COMPATIBILITY SameMajorVersion)

  install(
    FILES
    cmake/configmaps-config.cmake
      ${PROJECT_BINARY_DIR}/configmaps-config-version.cmake
    DESTINATION
      lib/cmake/configmaps
  )  
set(SOURCES 
    src/ConfigBase.cpp
    src/ConfigItem.cpp
    src/ConfigMap.cpp
    src/ConfigSchema.cpp
    src/ConfigVector.cpp
)
set(HEADERS
    src/ConfigAtom.hpp
    src/ConfigBase.hpp
    src/ConfigData.h
    src/ConfigItem.hpp
    src/ConfigMap.hpp
    src/ConfigSchema.hpp
    src/ConfigVector.hpp
    src/FIFOMap.h
)

add_library(configmaps SHARED ${SOURCES})
target_compile_features(configmaps PUBLIC cxx_std_17)
target_link_libraries(
        configmaps
        ${YAML_CPP_LIBRARIES}
        jsoncpp_lib
        pthread
        
)

target_include_directories(configmaps
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
		$<INSTALL_INTERFACE:include>
)


if(WIN32)
  set(LIB_INSTALL_DIR bin) # .dll are in PATH, like executables
else(WIN32)
  set(LIB_INSTALL_DIR lib)
endif(WIN32)

# Install the binaries
install(TARGETS configmaps EXPORT configmaps-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION lib
)

# Install headers into mars include directory
install(FILES ${HEADERS} DESTINATION include/configmaps)

# Prepare and install necessary files to support finding of the library 
install(
	FILES cmake/configmaps-config.cmake
	DESTINATION lib/cmake/configmaps
)


# using pkg-config
configure_file(configmaps.pc.in ${CMAKE_BINARY_DIR}/configmaps.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/configmaps.pc DESTINATION lib/pkgconfig)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

