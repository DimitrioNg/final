cmake_minimum_required(VERSION 2.8)
project(hserver)
#find_library(PTHREAD_LIBRARY pthread)
#find_library(PCRE_LIBRARY pcre)
#find_library(UNISTD_LIBRARY unistd)
include_directories(include)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread -lrt -Wall")
set(SRC_DIR src)
set(SRC_LIB_DIR ${SRC_DIR}/lib)
#set(SRC_LIST lib1/main.cpp lib2/main.cpp main.cpp)
set(SRC_LIST ${SRC_DIR}/main.cpp ${SRC_LIB_DIR}/daemonizator.cpp ${SRC_LIB_DIR}/errorproc.cpp ${SRC_LIB_DIR}/unblock.cpp ${SRC_LIB_DIR}/worker.cpp ${SRC_LIB_DIR}/fdtransceiver.cpp ${SRC_LIB_DIR}/webproc.cpp)
set(HEADER_FILES ${SRC_LIB_DIR}/fdtransceiver.h )
file(GLOB CPPS "*.cpp")
#set(SRC ${SRC_DIR} main.cpp)
#g++ -ggdb -O0 -o server server.cpp unblock.cpp error.cpp -lpthread -lrt

#target_link_libraries(${UNISTD_LIBRARY})

#add_executable(hserv src/main.cpp)
add_executable(../release/${PROJECT_NAME} ${SRC_LIST})

 