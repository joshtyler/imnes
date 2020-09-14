add_subdirectory(thirdparty)

add_executable(imnes main.cpp Cpu6502.cpp Cpu6502.h Cpu6502_instructions.h)
target_link_libraries(imnes PRIVATE project_options project_warnings magic_enum fmt)