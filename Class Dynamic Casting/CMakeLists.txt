cmake_minimum_required(VERSION 2.8.2)

project(Class__Dynamic_Casting)


add_executable(${PROJECT_NAME} main_test.cc Class_Dynamic_Casting.h)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)

