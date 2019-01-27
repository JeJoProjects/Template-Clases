cmake_minimum_required(VERSION 2.8.2)

project(SFINAE)


file(GLOB SOURCES
  *.h
  *.cpp
  *.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
