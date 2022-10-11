cmake_minimum_required ( VERSION 3.1.0 )

project ( VolumeRendering
  VERSION 3.0.0
  LANGUAGES CXX
)

# Set to c++14
set ( CMAKE_CXX_STANDARD 17 )

# Source
set ( SOURCE
  src/vendor/glm/glm.hpp
  src/vendor/glm/gtx/string_cast.hpp
  src/vendor/imgui/libs/imgui/imgui.h
  src/vendor/imgui/libs/imgui/imgui.cpp
  src/vendor/imgui/libs/imgui/backends/imgui_impl_glfw.h
  src/vendor/imgui/libs/imgui/backends/imgui_impl_glfw.cpp
  src/vendor/imgui/libs/imgui/backends/imgui_impl_opengl3.h
  src/vendor/imgui/libs/imgui/backends/imgui_impl_opengl3.cpp
  src/vendor/stb_image/stb_image.h
  src/vendor/stb_image/stb_image.cpp
  src/vendor/stb_image/stb_image_write.h
  src/vendor/stb_image/stb_image_write.cpp
  src/models/TestModel.cpp
  src/models/TestModel.h
  src/models/VolumeRendering.cpp
  src/models/VolumeRendering.h
  src/Application.cpp
  src/graphics/Camera.cpp
  src/graphics/Camera.h
  src/graphics/IndexBuffer.cpp
  src/graphics/IndexBuffer.h
  src/objects/UnitCube.cpp
  src/objects/UnitCube.h
  src/graphics/Renderer.cpp
  src/graphics/Renderer.h
  src/graphics/Shader.cpp
  src/graphics/Shader.h
  src/objects/VolumeTexture.cpp
  src/objects/VolumeTexture.h
  src/objects/Texture.cpp
  src/objects/Texture.h
  src/graphics/VertexArray.cpp
  src/graphics/VertexArray.h
  src/graphics/VertexBuffer.cpp
  src/graphics/VertexBuffer.h
  src/graphics/VertexBufferLayout.cpp
  src/graphics/VertexBufferLayout.h
  
)


# Executable
add_executable(VolumeRendering ${SOURCE})

# GLFW linking
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(VolumeRendering glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(VolumeRendering OpenGL::GL)

# IMGUI linking
add_subdirectory("src/vendor/imgui")
target_link_libraries(VolumeRendering IMGUI)

# GLEW linking
find_package(GLEW 2.0 REQUIRED)
target_link_libraries(VolumeRendering GLEW::GLEW)

# ASSIMP linking
find_package(assimp REQUIRED)
target_link_libraries(VolumeRendering assimp)

include_directories(src/vendor/imgui/libs/imgui/)
include_directories(src/graphics/)
include_directories(src/objects/)
include_directories(src/vendor/)