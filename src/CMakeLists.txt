cmake_minimum_required(VERSION 2.8.7)

include_directories(${SDSL_INCLUDES})
include_directories(${INCLUDES})
include_directories(${SDSL_DEPS_INCLUDES})

add_definitions(${COMPILE_WARNINGS})
add_definitions(-std=c++11 -O3)

add_library(libcdat "Alphabet.cpp"
                    "IndexBitVector.cpp"
                    "Index.cpp"
                    "IndexPerm.cpp"
                    "IndexWaveletTree.cpp")
add_dependencies(libcdat sdsl)

add_executable(cdat_build "cdat_build.cpp")
add_dependencies(cdat_build libcdat sdsl)
target_link_libraries(cdat_build libcdat sdsl ${Boost_LIBRARIES})
add_custom_command(TARGET cdat_build
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cdat_build> ../)

add_executable(cdat "cdat.cpp")
add_dependencies(cdat libcdat sdsl)
target_link_libraries(cdat libcdat sdsl ${Boost_LIBRARIES})
add_custom_command(TARGET cdat
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cdat> ../)

add_executable(cdat_check "cdat_check.cpp")
add_dependencies(cdat_check libcdat sdsl)
target_link_libraries(cdat_check libcdat sdsl ${Boost_LIBRARIES})
add_custom_command(TARGET cdat_check
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cdat_check> ../)