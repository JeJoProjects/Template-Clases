cmake_minimum_required(VERSION 3.0)

# separate projects
add_subdirectory (Algorithms)
set(JEJO_ALGO_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/Algorithms)

add_subdirectory (SFINAE)
set(JEJO_SFINAE_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/SFINAE)

add_subdirectory (JeJoProjects)
set(JEJO_TEMPLATE_PROJECTS_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoProjects)



set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "JeJoTemplateProjects")
