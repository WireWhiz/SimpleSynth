
add_library(simpleSynthEditor STATIC timelineEditor.cpp serializedData.cpp)
target_link_libraries(simpleSynthEditor PUBLIC graphics tfd jsoncpp_static)