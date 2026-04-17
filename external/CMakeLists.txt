add_subdirectory(glfw)

add_subdirectory(glad)

add_subdirectory(glm INTERFACE)
target_include_directories( glm INTERFACE glm )