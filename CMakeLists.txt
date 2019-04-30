cmake_minimum_required(VERSION 3.0)

project("Project_Template_Clases")

# separate projects
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_subdirectory (Algorithms)
set(JEJO_ALGO_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/Algorithms)

add_subdirectory (SFINAE)
set(JEJO_SFINAE_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/SFINAE)

add_subdirectory (JeJoProjects)
set(JEJO_TEMPLATE_PROJECTS_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoProjects)

# set start project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       PROPERTY VS_STARTUP_PROJECT "JeJoTemplateProjects")
