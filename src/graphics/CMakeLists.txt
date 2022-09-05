add_library(graphics STATIC window.cpp)
target_link_libraries(graphics PUBLIC glad imgui)
