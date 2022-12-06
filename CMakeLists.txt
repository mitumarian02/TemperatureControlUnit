cmake_minimum_required(VERSION 3.0.0)

project(Task_3_C++ VERSION 1.0.0)
add_executable(ACUnit
AC_Unit/main.cpp)
add_executable(thermostat
Thermostat/main.cpp)

find_package(Boost COMPONENTS thread system chrono REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(ACUnit ${Boost_LIBRARIES}
                            librt.so)
                            target_link_libraries(thermostat ${Boost_LIBRARIES}
                            librt.so)
set( CMAKE_CXX_FLAGS " -pthread -lrt -lpthread -lboost_thread" )