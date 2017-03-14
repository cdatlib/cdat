# Compressed direct-address tables

## Dependencies
Library has 3 dependencies:

    1. CMake
    2. Boost program options
    3. SDSL - provided in directory externals (https://github.com/simongog/sdsl-lite)

On Ubuntu you can install appropriate packages by typing:

```
sudo apt-get install cmake libboost-program-options-dev
```

## Library

Library provides 3 kinds of indexes: 

    1. Index with bit vector
    2. Index with reverse permutation
    3. Index with wavelet tree

There are 3 programs using this indexes:

    1. cdat_check - gives information about text with preferable word_size
    2. cdat_build - builds index from given text
    3. cdat - locates or counts occurrences of patterns in given index built by cdat_build

Example usage:

```
cmake .
make -j4
./cdat_build -i text/dna_4.200MB -o bit_index_12_3 -s 12 -f 3 -t bit
./cdat -i bit_index_12_3 -p reads/dna_4_200mb_100k_40.in -a locate -o locate_result.out
```

## Tools

    1. generate_pattern - generate random patterns from given text
    2. replace_dna - replaces all characters in text with 'A', 'C', 'T', 'G'
    
## Adding to project

You can add *CDAT* to your project by including header filer *cdat.hpp* to your sources and adding this lines to your *CMakeLists.txt* file:

```
add_definitions(-std=c++11)

add_subdirectory("cdat")
include_directories("cdat/include")

set(SDSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cdat/externals/sdsl-lite)
set(SDSL_DEPS_INCLUDES ${SDSL_DIR}/external/libdivsufsort-2.0.1/include)

include_directories(${SDSL_DEPS_INCLUDES})

add_executable(program_name "program_name.cpp")
add_dependencies(program_name libcdat sdsl)
target_link_libraries(program_name libcdat sdsl)
```
