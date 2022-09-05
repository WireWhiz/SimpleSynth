//
// Created by eli on 9/5/2022.
//

#ifndef SIMPLESYNTH_WINDOW_H
#define SIMPLESYNTH_WINDOW_H

#include <functional>

struct GLFWwindow;

class Window
{
	GLFWwindow* _window;
	bool _open = true;
public:
	void run(const std::function<void(int width, int height)>& callback);
	void close();
};


#endif //SIMPLESYNTH_WINDOW_H
