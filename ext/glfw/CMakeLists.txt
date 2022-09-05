add_library(glfw INTERFACE)
if(WIN32)
target_link_libraries(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib)
else()
target_link_libraries(glfw INTERFACE -lglfw)
endif()

target_include_directories(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)