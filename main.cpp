#include <iostream>
#include "src/graphics/window.h"
#include <imgui/imgui.h>
#include "src/synth/simpleSynth.h"
#include "src/synth/synthSource.h"

int main()
{
	SimpleSynth s;

	auto synthSource = std::make_unique<SynthSource>();
	synthSource->hertz = 440;

	s.device().addSource(synthSource.get());

	Window w;
	w.run([](int width, int height){
		ImGui::SetNextWindowPos({0,0});
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.2,0.2,0.2,1});
		ImGui::Begin("Simple Synth", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("Hello World");
		ImGui::End();
		ImGui::PopStyleColor();
	});
	s.device().removeSource(synthSource.get());

	return 0;
}
