# JanumachineTerrainFormat/jtf/CMakeList.txt

# create shared library target 'jtf'
add_library(jtf SHARED)

# versioning
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/jtf_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/jtf_version.h
    @ONLY
)

# include directories
target_include_directories(jtf PUBLIC
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

# source files
target_sources(jtf
    PRIVATE
        src/jtf_crc32.cpp
        src/jtf_reader.cpp
        src/jtf_writer.cpp
		src/jtf_c_api.cpp
		include/jtf_version.h)

# C++ standard
set_target_properties(jtf PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# define export macro for shared library
# on Windows, JTF_EXPORT will resolve to __declspec(dllexport)
# on other platforms, it can be empty or use visibility attributes
target_compile_definitions(jtf PRIVATE
	JTF_EXPORTS
)

# set consistent output names
set_target_properties(jtf PROPERTIES
	OUTPUT_NAME "jtf"
	PREFIX "lib"
)
if(WIN32)
	set_target_properties(jtf PROPERTIES
		PREFIX ""
	)
endif()

# version defines
target_compile_definitions(jtf PRIVATE
	JTF_VERSION_MAJOR=${JTF_VERSION_MAJOR}
    JTF_VERSION_MINOR=${JTF_VERSION_MINOR}
    JTF_VERSION_PATCH=${JTF_VERSION_PATCH}
)

# file metadata
# windows
if(WIN32)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/resource/jtf_version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/resource/jtf_version.rc
        @ONLY
	)
	target_sources(jtf PRIVATE 
		${CMAKE_CURRENT_BINARY_DIR}/resource/jtf_version.rc
	)
endif()
# macOS / Linux
set_target_properties(jtf PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)