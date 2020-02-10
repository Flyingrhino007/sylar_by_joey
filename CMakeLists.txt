cmake_minimum_required(VERSION 2.8)
project(sylar)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/apps/sylar/include)

link_directories(/apps/sylar/lib64)

find_library(YAMLCPP yaml-cpp)
#message("***",${YAMLCPP})
#message("***",${YAMLCPP_INCLUDE_DIR})

set(LIB_SRC
    src/log.cpp
    src/util.cc
    src/config.cc
    )

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar) #__FILE__


#Logger日志模块测试
add_executable(test tests/test.cpp)
add_dependencies(test sylar)
force_redefine_file_macro_for_sources(test) #__FILE__
target_link_libraries(test sylar ${YAMLCPP})

#Config配置模块（Yaml)测试
add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
force_redefine_file_macro_for_sources(test_config) #__FILE__
target_link_libraries(test_config sylar ${YAMLCPP})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

