add_subdirectory(magic_enum)
add_subdirectory(fmt)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

# Needed because arch doesn't install the static libs by default. Could remove later
set(BUILD_SHARED_LIBS ON)

# Needed to get ShowDemoWindow to work
set(IMGUI_SFML_IMGUI_DEMO ON)

add_subdirectory(imgui-sfml)