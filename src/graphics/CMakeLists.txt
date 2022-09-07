add_library(graphics STATIC window.cpp waveVisualizer.cpp waveVisualizer.h)
target_link_libraries(graphics PUBLIC glad imgui)
