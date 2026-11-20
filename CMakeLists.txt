cmake_minimum_required(VERSION 3.22.1)
project("mariaopenchess" CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# libnode.so e cabeçalhos do nodejs-mobile (baixados por: node android/scripts/prepare.js)
get_filename_component(LIBNODE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../libnode" ABSOLUTE)
include_directories("${LIBNODE_DIR}/include/node")

add_library(libnode SHARED IMPORTED)
set_target_properties(libnode PROPERTIES IMPORTED_LOCATION "${LIBNODE_DIR}/bin/${ANDROID_ABI}/libnode.so")

add_library(native-lib SHARED native-lib.cpp)
find_library(log-lib log)
target_link_libraries(native-lib libnode ${log-lib})
