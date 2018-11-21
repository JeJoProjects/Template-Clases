cmake_minimum_required(VERSION 2.8.2)

project(Template_Extra_Algorithums)

add_executable(${PROJECT_NAME} Template_count_adjacent_if.cpp)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
