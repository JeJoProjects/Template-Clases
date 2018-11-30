cmake_minimum_required(VERSION 2.8.2)

project(Template_Extra_Algorithums)

add_executable(${PROJECT_NAME} main_test.cc 
    Template_count_adjacent_if.h
    Template_get_adjacent_pairs_if.h
    Template_duplicates.h)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
