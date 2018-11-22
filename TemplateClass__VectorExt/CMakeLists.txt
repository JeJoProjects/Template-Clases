cmake_minimum_required(VERSION 2.8.2)

project(Template_Class_VectorExt)

add_executable(${PROJECT_NAME} Main_VectorExt.cc TemplateClass_VectorExt.h)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
