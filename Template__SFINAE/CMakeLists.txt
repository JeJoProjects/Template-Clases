cmake_minimum_required(VERSION 2.8.2)

project(SFINAE_Tests)

add_executable(${PROJECT_NAME} Main_Templates_SFINAE.cc Templates_SFINAE.h)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
