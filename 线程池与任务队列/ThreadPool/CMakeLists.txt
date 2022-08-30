cmake_minimum_required(VERSION 3.0.2)
project(ThreadPool)
add_compile_options(
  -std=c++11
  -std=c++14
  -Wreturn-type
  )
  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/bin)

include_directories(
  include
)

link_directories(
   ${PROJECT_SOURCE_DIR}/lib
   ${PROJECT_SOURCE_DIR}/thirdparty/spdlog/lib
)

set(SRC_FILE
    ${PROJECT_SOURCE_DIR}/src/LogComponent.cpp
    ${PROJECT_SOURCE_DIR}/src/Task.cpp
    ${PROJECT_SOURCE_DIR}/src/ThreadPool.cpp
    )

add_library(Thread STATIC 
            ${SRC_FILE}
            )
            
target_link_libraries(Thread  
            ${catkin_LIBRARIES}
            spdlog
            )

add_executable(ThreadPool_main excample/main.cpp)
target_link_libraries(ThreadPool_main Thread)
