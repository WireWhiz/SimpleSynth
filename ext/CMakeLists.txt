add_library(imgui STATIC
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/misc/fonts/binary_to_compressed_c.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_opengl2.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/misc/cpp/imgui_stdlib.cpp
        )
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
target_link_libraries(imgui PUBLIC glfw)

add_library(glad STATIC
        glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

add_subdirectory(glfw)

option(JSONCPP_WITH_TESTS OFF)
option(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)
option(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
option(JSONCPP_WITH_CMAKE_PACKAGE "Generate and install cmake package files" ON)
option(BUILD_SHARED_LIBS OFF)
add_subdirectory(jsoncpp)

add_subdirectory(tfd)

add_library(includes INTERFACE)
target_include_directories(includes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})