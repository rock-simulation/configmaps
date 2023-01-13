cmake_minimum_required(VERSION 3.5)
add_executable(configmaps_test 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/catch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests.cpp
)

target_include_directories(configmaps_test
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
		../src
)

target_compile_features(configmaps_test PUBLIC cxx_std_17) # Use C++17
# if(APPLE)
#   target_link_libraries(configmaps_test ${PKGCONFIGtest_LIBRARIES} ${CMAKE_BINARY_DIR}/libconfigmaps.dylib) #
# else(APPLE)
  target_link_libraries(configmaps_test PUBLIC
  configmaps
  )
# endif()

if(Catch_FOUND)
  catch_discover_tests(configmaps_test)
endif()

add_custom_target(test
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/configmaps_test
    DEPENDS configmaps
)

add_custom_command(TARGET configmaps_test PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/schema/ $<TARGET_FILE_DIR:configmaps>/schema)