find_package(yaml-cpp REQUIRED)
find_package(jsoncpp REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/configmaps-targets.cmake")
link_directories(${YAML_CPP_LIBRARY_DIR})