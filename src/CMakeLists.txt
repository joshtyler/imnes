add_subdirectory(thirdparty)

add_executable(imnes main.cpp Cpu6502.cpp Cpu6502.h Cpu6502_instructions.h ines.cpp ines.h)

# Mark the thirdparty include directories as system so that we don't get project warnings applied to the headers
# (imgui is a particular culprit)
# See https://stackoverflow.com/questions/52135983
function(target_link_libraries_system target)
    set(libs ${ARGN})
    foreach(lib ${libs})
        get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
        target_include_directories(${target} SYSTEM PRIVATE ${lib_include_dirs})
        target_link_libraries(${target} PRIVATE ${lib})
    endforeach(lib)
endfunction(target_link_libraries_system)
target_link_libraries_system(imnes fmt ImGui-SFML magic_enum imgui_memory_editor)

target_link_libraries(imnes PRIVATE project_options project_warnings)
