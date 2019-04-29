cmake_minimum_required(VERSION 3.0)

# seperate projects
add_subdirectory (SFINAE)
add_subdirectory (Algorithms)
add_subdirectory (JeJoProjects)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "JeJoTemplateProjects")
