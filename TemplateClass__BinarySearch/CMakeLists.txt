cmake_minimum_required(VERSION 2.8.2)

project(Class__BinarySearch)

add_executable(${PROJECT_NAME} main_test.cc TemplateClass_BinarySearch.h)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
