#include <iostream>
#include "src/graphics/window.h"
#include <imgui/imgui.h>
#include <string>
#include "src/synth/simpleSynth.h"
#include "src/synth/sinSynth.h"
#include "src/synth/compressorSource.h"
#include "src/synth/ampSource.h"
#include "src/synth/squareSynth.h"
#include "src/synth/sawSynth.h"
#include "src/graphics/waveVisualizer.h"
#include "src/synth/timelineSource.h"
#include "src/windows/timelineEditor.h"

int main()
{
    Window w;
	SimpleSynth s;

	/*SinSynth sinSynth;
	sinSynth.startSample = 0;
	sinSynth.hertz = 261.63;

	SquareSource squareSynth;
	SawSynth sawSynth;

	TimelineSource timeline;
	timeline.bpm = 250;
	timeline.beats.resize(8);
	timeline.loop = true;
	timeline.synth = &sinSynth;

	AmpSource amp(&timeline);
	amp.amplitude = 0.1;

	WaveVisualizer visual(&timeline);

	s.device().addSource(&amp);*/

	TimelineEditor editor(&s.device());

	w.run([&](int width, int height){
		ImGui::SetNextWindowPos({0,0});
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.2,0.2,0.2,1});
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
		ImGui::Begin("Simple Synth", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);

		editor.draw();

		/*float hertz = sinSynth.hertz;
		ImGui::SliderFloat("Hertz ", &hertz, 20, 15000);
		sinSynth.hertz = hertz;
		squareSynth.hertz = hertz;
		sawSynth.hertz = hertz;


		if(ImGui::Button("Sin Wave"))
			timeline.synth = &sinSynth;
		ImGui::SameLine();
		if(ImGui::Button("Square Wave"))
			timeline.synth = &squareSynth;
		ImGui::SameLine();
		if(ImGui::Button("Saw Wave"))
			timeline.synth = &sawSynth;

		if(ImGui::CollapsingHeader("Visualization"))
		{
			ImGui::DragFloat("##RangeStart", &visual.startTime);
			ImGui::DragFloat("Range (ms)", &visual.endTime);
			visual.draw();
		}
		if(ImGui::CollapsingHeader("Timeline"))
		{
			int beatCount = timeline.beats.size();
			ImGui::InputInt("Beat Count", &beatCount);
			if(ImGui::IsItemDeactivatedAfterEdit())
				timeline.beats.resize(beatCount);
			ImGui::InputFloat("BPM", &timeline.bpm);
			ImGui::InputInt2("Octave Range", octaveRange);
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Relative to the octave containing C4, notes will not be deleted in unused octaves on resize");
			if(octaveRange[0] > octaveRange[1])
				octaveRange[1] = octaveRange[0];
			if(ImGui::BeginTable("Note Plotter", timeline.beats.size() + 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_PreciseWidths | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX))
			{
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoReorder, 60);
				for(size_t i = 0; i < timeline.beats.size(); ++i)
					ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoReorder, 80);

				int selectableID = 0;
				for(int octave = octaveRange[1]; octave >= octaveRange[0]; --octave)
				{
					for(auto& note : notes)
					{
						float hertz = note.second;
						if(octave < 0)
							hertz = hertz * std::pow(2, 1 / (-1.0f * (float)octave));
						if(0 < octave)
							hertz = hertz * std::pow(2, octave);
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("%s ", note.first.c_str(), octave);
						for(auto& beat : timeline.beats)
						{
							ImGui::TableNextColumn();
							ImGui::PushID(selectableID++);
							bool hasNote = beat.notes.count(hertz);
							if(ImGui::Selectable("##", hasNote))
							{
								if(hasNote)
									beat.notes.erase(hertz);
								else
									beat.notes.insert(hertz);
							}
							ImGui::PopID();
						}
					}
					if(octave != octaveRange[0])
						ImGui::TableNextRow(); //Add spacing between octaves
				}
				ImGui::EndTable();
			}
		}*/

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	});
	return 0;
}
