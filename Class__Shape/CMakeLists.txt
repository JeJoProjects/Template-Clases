cmake_minimum_required(VERSION 2.8.2)

project(Class__Shape)

add_executable(${PROJECT_NAME} main_test.cc Class_Shape.h)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
