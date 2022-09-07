#include <iostream>
#include "src/graphics/window.h"
#include <imgui/imgui.h>
#include "src/synth/simpleSynth.h"
#include "src/synth/sinSource.h"
#include "src/synth/compressorSource.h"
#include "src/synth/ampSource.h"
#include "src/synth/squareSource.h"
#include "src/synth/sawSource.h"
#include "src/graphics/waveVisualizer.h"

int main()
{
	SimpleSynth s;

	SinSource sinSynth;
	sinSynth.startSample = 0;
	sinSynth.hertz = 261.63;

	SquareSource squareSynth;
	SawSource sawSynth;


	CompressorSource compressor(&sinSynth);
	AmpSource amp(&compressor);
	amp.amplitude = 0.1;

	WaveVisualizer visual(&compressor);

	s.device().addSource(&amp);


	Window w;
	w.run([&compressor, &sinSynth, &squareSynth, &sawSynth, &visual](int width, int height){
		ImGui::SetNextWindowPos({0,0});
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.2,0.2,0.2,1});
		ImGui::Begin("Simple Synth", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		int resolution = compressor.resolution;
		ImGui::InputInt("Compressor Resolution", &resolution);
		compressor.resolution = resolution;

		float hertz = sinSynth.hertz;
		ImGui::SliderFloat("Hertz ", &hertz, 20, 15000);
		sinSynth.hertz = hertz;
		squareSynth.hertz = hertz;
		sawSynth.hertz = hertz;


		if(ImGui::Button("Sin Wave"))
			compressor.source = &sinSynth;
		ImGui::SameLine();
		if(ImGui::Button("Square Wave"))
			compressor.source = &squareSynth;
		ImGui::SameLine();
		if(ImGui::Button("Saw Wave"))
			compressor.source = &sawSynth;

		if(ImGui::CollapsingHeader("Visualization"))
		{
			ImGui::DragFloat("##RangeStart", &visual.startTime);
			ImGui::DragFloat("Range (ms)", &visual.endTime);
			visual.draw();
		}

		ImGui::End();
		ImGui::PopStyleColor();
	});
	s.device().removeSource(&amp);
	return 0;
}
