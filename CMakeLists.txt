cmake_minimum_required(VERSION 3.2)

project("JeJo_Templates")

# separate projects
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_subdirectory (JeJoAlgorithums)
set(JEJO_ALGO_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoAlgorithums)

add_subdirectory (JeJoTemplateClasses)
set(JEJO_TEMPLATE_PROJECTS_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoTemplateClasses)

add_subdirectory (JeJoTraits_and_SFINE)
set(JEJO_SFINAE_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoTraits_and_SFINE)


# set start project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       PROPERTY VS_STARTUP_PROJECT "JeJoTraits_and_SFINE")
