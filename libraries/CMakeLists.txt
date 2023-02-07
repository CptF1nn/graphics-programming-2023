add_subdirectory(glad)

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")
add_subdirectory(glfw)
add_subdirectory(itugl)
add_subdirectory(stb)
