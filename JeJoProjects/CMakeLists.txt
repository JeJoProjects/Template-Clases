 project("JeJoTemplateProjects")

file(GLOB SOURCES
    *.h
    *.hpp
    *.cc
    *.cpp
)
include_directories(${JEJO_ALGO_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${SOURCES})

#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
#install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
