cmake_minimum_required(VERSION 3.20)

project("JeJo_Templates")

# separate projects
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# multi process compiler
if(MSVC) # AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.12.25835)
       set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
       set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /Zi /O2")

       message ("----------------------- MSVC compiler: " ${CMAKE_CXX_COMPILER_VERSION} " ------------------------")
       message ("------------- CXX Flags:" ${CMAKE_CXX_FLAGS} " -------------")

else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)

       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O2 -Wall -Wextra") 

       message ("----------------------- GCC compiler: " ${CMAKE_CXX_COMPILER_VERSION} " ------------------------")
       message ("------------- CXX Flags:" ${CMAKE_CXX_FLAGS} " -------------")
endif()

add_subdirectory (JeJoAlgorithums)
set(JEJO_ALGO_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoAlgorithums)

add_subdirectory (JeJoTemplateClasses)
set(JEJO_TEMPLATE_PROJECTS_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoTemplateClasses)

add_subdirectory (JeJoTraits_and_SFINE)
set(JEJO_SFINAE_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/JeJoTraits_and_SFINE)


# set start project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       PROPERTY VS_STARTUP_PROJECT "JeJoTraits_and_SFINE")
