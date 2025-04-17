# CMakeList.txt : CMake project for EvilDungeons, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("EvilDungeons")

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1 
)
FetchContent_MakeAvailable(glm)

# IMGUI
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.90.4
)
FetchContent_MakeAvailable(imgui)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  src/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
  include/*.hpp include/*.h
)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# Add source to this project's executable.
add_executable (EvilDungeons ${SOURCES} ${HEADERS})

target_include_directories(EvilDungeons PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(EvilDungeons
  PRIVATE 
  glfw
  glad
  glm
  imgui
)
target_include_directories(EvilDungeons PRIVATE ${glad_SOURCE_DIR}/include)


if (WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
else()
  find_package(OpenGL REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EvilDungeons PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
