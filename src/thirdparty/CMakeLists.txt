# Build static libs so that the executable will play nicely with vanilla windows
# Could package with built .dll files if it gets massive
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(sfml)
add_subdirectory(magic_enum)
add_subdirectory(fmt)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(IMGUI_SFML_IMGUI_DEMO ON) # Needed to get ShowDemoWindow to work
set(IMGUI_SFML_FIND_SFML OFF) # We have built SFML locally
add_subdirectory(imgui-sfml)
