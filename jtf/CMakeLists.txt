# JanumachineTerrainFormat/jtf/CMakeList.txt

# create shared library target 'jtf'
add_library(jtf SHARED)

# include directories
target_include_directories(jtf
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# source files
target_sources(jtf
    PRIVATE
        src/jtf_crc32.cpp
        src/jtf_reader.cpp
        src/jtf_writer.cpp
		src/jtf_c_api.cpp
)

# C++ standard
set_target_properties(jtf PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Define export macro for shared library
# On Windows, JTF_EXPORT will resolve to __declspec(dllexport)
# On other platforms, it can be empty or use visibility attributes
target_compile_definitions(jtf
    PRIVATE JTF_EXPORTS
)