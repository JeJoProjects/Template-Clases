cmake_minimum_required(VERSION 2.8.2)

project(Template__Class__PairExt.h)

add_executable(${PROJECT_NAME} main_test.cc TemplateClass__PairExt.h)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
