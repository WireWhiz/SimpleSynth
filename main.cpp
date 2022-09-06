#include <iostream>
#include "src/graphics/window.h"
#include <imgui/imgui.h>
#include "src/synth/simpleSynth.h"
#include "src/synth/synthSource.h"

int main()
{
	SimpleSynth s;

	auto synthSourceC = std::make_unique<SynthSource>();
	synthSourceC->startSample = 0;
	synthSourceC->hertz = 261.63;
	synthSourceC->volume = 0.3;

	auto synthSourceE = std::make_unique<SynthSource>();
	synthSourceE->startSample = 30;
	synthSourceE->hertz = 329.63;
	synthSourceE->volume = 0.3;

	auto synthSourceG = std::make_unique<SynthSource>();
	synthSourceC->startSample = 10;
	synthSourceG->hertz = 392;
	synthSourceG->volume = 0.3;

	s.device().addSource(synthSourceC.get());
	s.device().addSource(synthSourceE.get());
	s.device().addSource(synthSourceG.get());

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
	s.device().removeSource(synthSourceC.get());
	s.device().removeSource(synthSourceE.get());
	s.device().removeSource(synthSourceG.get());
	return 0;
}
