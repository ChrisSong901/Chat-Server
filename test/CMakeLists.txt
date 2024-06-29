cmake_minimum_required(VERSION 3.0)

project(main)

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

include_directories(${PROJECT_SOURCE_DIR}/thirdpart)

#link_libraries()


set(SRC_LIST ./muduo_server.cpp)


#设置可执行文件最终位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


#根据哪些生成可执行文件
add_executable(server ${SRC_LIST})


#链接的库
target_link_libraries(server muduo_net muduo_base pthread)
