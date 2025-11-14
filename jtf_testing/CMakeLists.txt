# JanumachineTerrainFormat/jtf_testing/CMakeList.txt

# test executable
add_executable(jtf_testing
    jtf_testing.cpp
)

# link to the jtf shared library
target_link_libraries(jtf_testing
    PRIVATE
        jtf
)

# include directories are automatically inherited from the jtf target (since jtf declares its PUBLIC include path)

# ensure consistent language standard (optional)
set_target_properties(jtf_testing PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# optional Windows tweak: ensure .dll is copied next to the .exe after build
if (WIN32)
    add_custom_command(TARGET jtf_testing POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:jtf>
            $<TARGET_FILE_DIR:jtf_testing>
    )
endif()