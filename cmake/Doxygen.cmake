# File is from here: https://github.com/lefticus/cpp_starter_project/blob/master/cmake/
# (cpp_starter_project uses Unlicense, so is compatible with this project)

function(enable_doxygen)
    option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" OFF)
    if(ENABLE_DOXYGEN)
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        find_package(Doxygen REQUIRED dot)
        doxygen_add_docs(doxygen-docs ${PROJECT_SOURCE_DIR})

    endif()
endfunction()
