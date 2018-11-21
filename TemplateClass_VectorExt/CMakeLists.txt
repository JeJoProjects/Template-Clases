cmake_minimum_required(VERSION 2.8.2)

project(Template_Class_VectorExt)

add_executable(${PROJECT_NAME} TemplateClass_VectorExt.cpp)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
