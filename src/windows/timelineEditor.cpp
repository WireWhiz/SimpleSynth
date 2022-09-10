//
// Created by eli on 9/9/2022.
//

#include "timelineEditor.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "../synth/sinSynth.h"
#include "../synth/squareSynth.h"
#include "../synth/sawSynth.h"
#include "../graphics/waveVisualizer.h"
#include "imgui_internal.h"
#include "../synth/soundDevice.h"

std::vector<std::pair<std::string, float>> baseNotes = {
		{"F#", 370.0f},
		{"F",  349.2f},
		{"E",  329.6f},
		{"D#", 311.1f},
		{"D",  293.7f},
		{"C#", 277.2f},
		{"C",  261.6f},
		{"B",  246.9f},
		{"A#", 233.1f},
		{"A",  220.0f},
		{"G#", 207.7f},
		{"G",  196.0f},
};

class TimelineEditorPlayer : public SoundSource
{
	TimelineEditor& _editor;
	double _lastTime = 0;
public:
	TimelineEditorPlayer(TimelineEditor& editor) : _editor(editor){};
	Sample getSample(double currentTime) override
	{
		Sample s{0,0};
		std::scoped_lock lock(_editor._mutex);
		if(_editor._playing)
		{
			_editor._currentTime += currentTime - _lastTime;
			for(auto& t : _editor._timelines)
			{
				s += t.timeline.getSample(_editor._currentTime) * t.volume;
			}
		}
		_lastTime = currentTime;
		return s;
	}
};

TimelineEditor::TimelineEditor(SoundDevice* device)
{
	_device = device;

	for(int octave = 3; octave >= -3; --octave)
	{
		for(auto& note : baseNotes)
		{
			float hertz = note.second;
			if(octave < 0)
				hertz = hertz * std::pow(2, 1 / (-1.0f * (float)octave));
			if(0 < octave)
				hertz = hertz * std::pow(2, octave);
			_allNotes.emplace_back(std::to_string(octave + 4) + " " + note.first, hertz);
		}
	}
	_player = new TimelineEditorPlayer(*this);
	_device->addSource(_player);
}

TimelineEditor::~TimelineEditor()
{
	_device->removeSource(_player);
	delete _player;
}

void TimelineEditor::draw()
{
	std::scoped_lock lock(_mutex);
	if(ImGui::BeginTable("MenuSplit", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoHostExtendY | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX, ImGui::GetContentRegionAvail()))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4,4));
		drawMenu();
		ImGui::PopStyleVar();
		ImGui::TableNextColumn();
		drawNotePickerWindow();
		ImGui::EndTable();
	}
}

void TimelineEditor::drawMenu()
{
	ImGui::BeginChild("Menu", {0,0}, true);

	std::vector<const char*> timelineNames;
	for(auto& t : _timelines)
		timelineNames.push_back(t.name.c_str());
	const char* selectedName = "select timeline";
	if(_selectedTimeline >= 0)
		selectedName = _timelines[_selectedTimeline].name.c_str();

	if(ImGui::BeginCombo("Timeline", selectedName))
	{
		if(_timelines.empty())
			ImGui::Selectable("select timeline");
		for(int i = 0; i < _timelines.size(); ++i)
		{
			if(ImGui::Selectable(_timelines[i].name.c_str(), i == _selectedTimeline))
				_selectedTimeline = i;
		}
		ImGui::EndCombo();
	}
	if(ImGui::Button("new##timeline"))
	{
		TimelineCtx ctx;
		ctx.name = "new timeline";
		_timelines.push_back(std::move(ctx));
		_selectedTimeline = _timelines.size() - 1;
	}
	ImGui::SameLine();
	if(ImGui::Button("delete##timeline"))
		ImGui::OpenPopup("delete timeline");

	if(ImGui::BeginPopup("delete timeline"))
	{
		ImGui::Text("Delete %s?", _timelines[_selectedTimeline].name.c_str());
		if(ImGui::Button("yes"))
		{
			_timelines.erase(_timelines.begin() + _selectedTimeline);
			_selectedTimeline--;
			if(_selectedTimeline < 0 && !_timelines.empty())
				_selectedTimeline = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if(ImGui::Button("no"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	ImGui::Separator();
	if(ImGui::CollapsingHeader("Timeline Settings"))
	{
		ImGui::PushID("Timeline Settings");
		ImGui::Indent(7);
		if(_selectedTimeline >= 0)
		{
			auto& timeline = _timelines[_selectedTimeline];
			ImGui::InputText("Name##timeline", &timeline.name);

			int beatCount = timeline.timeline.beats.size();
			ImGui::InputInt("Beat Count", &beatCount);
			if(ImGui::IsItemDeactivatedAfterEdit())
				timeline.timeline.beats.resize(beatCount);
			ImGui::InputFloat("BPM", &timeline.timeline.bpm);
			ImGui::SliderFloat("Volume", &timeline.volume, 0, 1, "%.2f");

			ImGui::Separator();
			const char* synthName = "none";
			if(timeline.synth >= 0)
				synthName = _synths[timeline.synth].name.c_str();
			if(ImGui::BeginCombo("Synth", synthName))
			{
				for(int i = 0; i < _synths.size(); ++i)
				{
					if(ImGui::Selectable(_synths[i].name.c_str(), i == timeline.synth))
					{
						timeline.synth = i;
						timeline.timeline.synth = _synths[i].synth.get();
					}
				}
				ImGui::EndCombo();
			}
			if(ImGui::Button("new##synth"))
			{
				SynthCtx ctx;
				ctx.name = "new synth";
				_synths.push_back(std::move(ctx));
				timeline.synth = _synths.size() - 1;
				timeline.timeline.synth = nullptr;
			}
			ImGui::SameLine();
			if(ImGui::Button("delete##synt"))
				ImGui::OpenPopup("delete synth");

			if(ImGui::BeginPopup("delete synth"))
			{
				ImGui::Text("Delete %s?", _synths[timeline.synth].name.c_str());
				if(ImGui::Button("yes"))
				{
					auto s = timeline.synth;
					_synths.erase(_synths.begin() + s);
					for(auto& t : _timelines)
					{
						if(t.synth == s)
						{
							t.synth = -1;
							t.timeline.synth = nullptr;
						}
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if(ImGui::Button("no"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			if(timeline.synth >= 0 && ImGui::CollapsingHeader("Synth Settings"))
			{
				ImGui::Indent(7);
				auto& synth = _synths[timeline.synth];
				ImGui::InputText("Name##synth", &synth.name);

				const char* synthType = nullptr;
				if(!synth.synth)
					synthType = "none";
				else if(dynamic_cast<SinSynth*>(synth.synth.get()))
					synthType = "Sin Synth";
				else if(dynamic_cast<SquareSynth*>(synth.synth.get()))
					synthType = "Square Synth";
				else if(dynamic_cast<SawSynth*>(synth.synth.get()))
					synthType = "Saw Synth";

				if(ImGui::BeginCombo("Type", synthType))
				{
					if(ImGui::Selectable("Sin Synth"))
						synth.synth = std::make_unique<SinSynth>();
					if(ImGui::Selectable("Square Synth"))
						synth.synth = std::make_unique<SquareSynth>();
					if(ImGui::Selectable("Saw Synth"))
						synth.synth = std::make_unique<SawSynth>();
					ImGui::EndCombo();
				}

				if(synth.synth)
				{
					synth.synth->hertz = 50;
					WaveVisualizer visualizer(synth.synth.get());
					visualizer.endTime = 150;
					visualizer.draw();
				}

				ImGui::Unindent(7);
			}
		}
		else
			ImGui::TextDisabled("No timeline selected");
		ImGui::PopID();
		ImGui::Unindent(7);
	}


	ImGui::EndChild();
}

void TimelineEditor::drawNotePickerWindow()
{
	ImGui::BeginChild("Note Picker Window", {0,0}, 0);
	if(ImGui::BeginTable("Play Menu Div", 1, ImGuiTableFlags_BordersInnerH))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if(ImGui::Button(_playing ? "Pause" : "Play", {60, 0}))
			_playing = !_playing;
		ImGui::SameLine();
		ImGui::DragScalar("Time", ImGuiDataType_Double, &_currentTime, 0.1f, nullptr, nullptr, "%.2lf");
		ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - 3);
		ImGui::SliderFloat("##Scroll", &_noteScrollPos, 0, _noteScrollMax, "");
		if(ImGui::IsItemEdited())
			_noteScrollSet = true;

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if(_selectedTimeline < 0)
		{
			auto windowSize = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPos({windowSize.x / 2, windowSize.y / 2});
			ImGui::TextDisabled("No timeline selected");
		} else
			drawNotePicker();

		ImGui::EndTable();
	}
	ImGui::EndChild();
}

void TimelineEditor::drawNotePicker()
{
	ImGui::BeginChild("Note Picker", {0,ImGui::GetContentRegionAvail().y - 2}, false);
	ImVec2 noteSize = {80, 18};
	if(ImGui::BeginTable("Note Names", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadOuterX, {40, 0}))
	{
		ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 40);

		for(auto& note : _allNotes)
		{
			ImGui::TableNextRow(0, noteSize.y);
			ImGui::TableNextColumn();
			ImGui::Text("%s", note.first.c_str());
		}

		ImGui::EndTable();
	}
	ImGui::SameLine(0,0);
	ImGui::BeginChild("Note Picker Scroll", {0, noteSize.y * _allNotes.size()});

	if(_noteScrollSet)
	{
		ImGui::SetScrollX(_noteScrollPos);
		_noteScrollSet = false;
	}
	else
		_noteScrollPos = ImGui::GetScrollX();
	_noteScrollMax = ImGui::GetScrollMaxX();
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0,0});
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0,0});
	auto& timeline = _timelines[_selectedTimeline].timeline;
	size_t beats = timeline.beats.size();
	size_t beatIndex = 0;
	int selectableID = 0;
	while(beats > 0)
	{
		size_t beatChunk = std::min<size_t>(64, beats);
		ImGui::SameLine(0,0);
		if(ImGui::BeginTable("Note Picker Grid", (int)beatChunk, ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadOuterX, {beatChunk*noteSize.x, noteSize.y * _allNotes.size()}))
		{
			for(size_t i = 0; i < beatChunk; ++i)
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoReorder, noteSize.x);
			for(auto& note : _allNotes)
			{
				ImGui::TableNextRow(0, noteSize.y);
				for(size_t i = 0; i < beatChunk; ++i)
				{
					ImGui::TableNextColumn();
					ImGui::PushID(selectableID++);
					auto& beat = timeline.beats[beatIndex + i];
					bool hasNote = beat.notes.count(note.second);
					if(ImGui::Selectable("##", hasNote, ImGuiSelectableFlags_NoPadWithHalfSpacing, noteSize))
					{
						if(hasNote)
							beat.notes.erase(note.second);
						else
							beat.notes.insert(note.second);
					}
					ImGui::PopID();
				}
			}
			beatIndex += beatChunk;
			beats -= beatChunk;
			ImGui::EndTable();
		}
	}

	ImGui::PopStyleVar(2);
	ImGui::EndChild();
	ImGui::EndChild();
}




