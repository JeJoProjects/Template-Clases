project("SFINAE")

file(GLOB SOURCES
   *.h
   *.hpp
   *.cc
   *.cpp
 )
#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
add_executable(${PROJECT_NAME} ${SOURCES})
#install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)