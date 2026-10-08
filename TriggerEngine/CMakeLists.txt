# CMakeList.txt : CMake project for TriggerEngine, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (TriggerEngine "source/main.cpp" "source/main.h" )

# This creates a separate Snake Game program
add_executable(SnakeStandalone "source/SnakeGame.cpp" "source/SnakeGame.h")

# This creates a separate Mini Render Lab program
add_executable(MiniRenderLab "source/MiniRenderLab.cpp" "source/MiniRenderLab.h")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TriggerEngine PROPERTY CXX_STANDARD 20)
  set_property(TARGET SnakeStandalone PROPERTY CXX_STANDARD 20)
   set_property(TARGET MiniRenderLab PROPERTY CXX_STANDARD 20)
endif()

# adding glfw libariy

add_subdirectory(thirdparty/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
include_directories(thirdparty/glfw-3.4/include)

# Add Glew library
set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
add_subdirectory(thirdparty/glew/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
include_directories(thirdparty/glew/include)
set_property(TARGET glew_s PROPERTY FOLDER GLEW)
set_property(TARGET glew   PROPERTY FOLDER GLEW)

#
#target_link_libraries(${PROJECT_NAME} 
 #   glfw 
  #  glew_s
#)

target_link_libraries(TriggerEngine 
    glfw 
    glew_s
)

target_link_libraries(SnakeStandalone 
    glfw 
    glew_s
)
target_link_libraries(MiniRenderLab 
    glfw 
    glew_s
)

# TODO: Add tests and install targets if needed.
