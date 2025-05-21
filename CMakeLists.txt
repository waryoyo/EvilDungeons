# CMakeList.txt : CMake project for EvilDungeons, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

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

# ASSIMP
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.4.3
)

FetchContent_MakeAvailable(assimp)

file(GLOB IMGUI_SRC
    ${imgui_SOURCE_DIR}/*.cpp
)

set(IMGUI_BACKENDS
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC} ${IMGUI_BACKENDS})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}
${imgui_SOURCE_DIR}/backends
${glfw_SOURCE_DIR}/include)



file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  src/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
  include/*.hpp include/*.h
)

add_library(glad STATIC external/glad/src/glad.c)
add_library(stb_image STATIC external/stb/stb_image.cpp)

add_library(FastNoiseLite STATIC external/fastNoise/FastNoiseLite.cpp)

target_include_directories(stb_image PUBLIC external/stb)
target_include_directories(glad PUBLIC external/glad/include)
target_include_directories(FastNoiseLite PUBLIC external/fastNoise)

# Add source to this project's executable.
add_executable (EvilDungeons ${SOURCES} ${HEADERS})

add_custom_command(TARGET EvilDungeons POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:EvilDungeons>/assets
)

add_custom_command(TARGET EvilDungeons POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/shaders
          $<TARGET_FILE_DIR:EvilDungeons>/shaders
)

target_include_directories(EvilDungeons PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(EvilDungeons
  PRIVATE 
  glfw
  glad
  glm
  imgui
  stb_image
  assimp::assimp
  FastNoiseLite
)
target_include_directories(EvilDungeons PRIVATE ${glad_SOURCE_DIR}/include)


if (WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
else()
  find_package(OpenGL REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EvilDungeons PROPERTY CXX_STANDARD 20)
endif()

if (NOT MSVC)
  target_compile_options(assimp PRIVATE
    -Wno-unknown-pragmas
    -Wno-error=unknown-pragmas
  )
endif()

add_compile_definitions(__USE_MINGW_ANSI_STDIO=1)


# TODO: Add tests and install targets if needed.
