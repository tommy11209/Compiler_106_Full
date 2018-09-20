cmake_minimum_required(VERSION 3.1)

project(p-compiler)
find_package(FLEX REQUIRED)
if(FLEX_FOUND)
    FLEX_TARGET(scanner 0413249_hw1.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.c)
    include_directories(${FLEX_INCLUDE_DIRS})
    add_executable(output scanner.c)

    target_link_libraries(output ${FLEX_LIBRARIES})
    
endif(FLEX_FOUND)
