#include <iostream>
#include "src/graphics/window.h"
#include <imgui/imgui.h>
#include "src/synth/simpleSynth.h"

int main()
{
	SimpleSynth s;

	Window w;
	w.run([](int width, int height){
		ImGui::SetNextWindowPos({0,0});
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (.2,.2,.2,1));
		ImGui::Begin("Simple Synth", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("Hello World");
		ImGui::End();
		ImGui::PopStyleColor();
	});
	return 0;
}
