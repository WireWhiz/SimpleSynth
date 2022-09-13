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
#include "jsoncpp/include/json/json.h"
#include <fstream>
#include "serializedData.h"
#include "tinyfiledialogs.h"

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
		float octaveMult = 1;
		if(octave < 0)
			octaveMult = 1.0f / std::pow(2.0f, (-1.0f * (float)octave));
		if(0 < octave)
			octaveMult = std::pow(2.0f, (float)octave);
		for(auto& note : baseNotes)
			_allNotes.emplace_back(std::to_string(octave + 4) + " " + note.first, note.second * octaveMult);
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
	ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.2,0.2,0.2,1});
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
	ImGui::Begin("Simple Synth", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3,3));
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::Selectable("Open"))
			{
				const char* filter[] = {"*.synth"};
				const char* promptRes = tinyfd_openFileDialog("Open Project", std::filesystem::current_path().parent_path().string().c_str(), 1, filter, "Simple Synth", 0);
				if(promptRes)
					loadProject(promptRes);
			}
			if(ImGui::Selectable("Save"))
				saveProject();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::PopStyleVar();

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

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
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
						timeline.timeline.synth = _synths[i].source.get();
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
				if(!synth.source)
					synthType = "none";
				else if(dynamic_cast<SinSynth*>(synth.source.get()))
					synthType = "Sin Synth";
				else if(dynamic_cast<SquareSynth*>(synth.source.get()))
					synthType = "Square Synth";
				else if(dynamic_cast<SawSynth*>(synth.source.get()))
					synthType = "Saw Synth";

				bool synthCreated = false;
				if(ImGui::BeginCombo("Type", synthType))
				{
					if(ImGui::Selectable("Sin Synth"))
					{
						synth.source = std::make_unique<SinSynth>();
						synthCreated = true;
					}
					if(ImGui::Selectable("Square Synth"))
					{
						synth.source = std::make_unique<SquareSynth>();
						synthCreated = true;
					}
					if(ImGui::Selectable("Saw Synth"))
					{
						synth.source = std::make_unique<SawSynth>();
						synthCreated = true;
					}
					ImGui::EndCombo();
				}
				if(synthCreated)
					timeline.timeline.synth = synth.source.get();

				if(synth.source)
				{
					synth.source->hertz = 50;
					WaveVisualizer visualizer(synth.source.get());
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
	ImColor sharpColor = ImColor(0.20f, 0.40f, 0.30f, 0.34f);
	if(ImGui::BeginTable("Note Names", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadOuterX, {40, 0}))
	{
		ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 40);

		for(auto& note : _allNotes)
		{
			ImGui::TableNextRow(0, noteSize.y);
			ImGui::TableNextColumn();
			if(note.first[note.first.size() - 1] == '#')
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, sharpColor);
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
		ImGui::SameLine(0, 0);
		if(ImGui::BeginTable(("Note Picker Grid" + std::to_string(beatIndex)).c_str(), (int)beatChunk, ImGuiTableFlags_Borders | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoHostExtendX, {beatChunk*(noteSize.x + 1), noteSize.y * _allNotes.size()}))
		{
			for(size_t i = 0; i < beatChunk; ++i)
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoReorder |  ImGuiTableFlags_RowBg, noteSize.x);
			for(auto& note : _allNotes)
			{
				bool isSharp = note.first[note.first.size() - 1] == '#';
				if(isSharp)
				{
					/*ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.16f, 0.48f, 0.29f, 0.54f));
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0.16f, 0.48f, 0.29f, 0.54f));*/
				}
				ImGui::TableNextRow(0, noteSize.y);

				for(size_t i = 0; i < beatChunk; ++i)
				{
					ImGui::TableNextColumn();
					if(isSharp)
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, sharpColor);
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
					if(ImGui::IsItemHovered())
					{
						if(hasNote && ImGui::IsMouseDown(ImGuiMouseButton_Right))
							beat.notes.erase(note.second);
					}
					ImGui::PopID();
				}
				/*if(isSharp)
					ImGui::PopStyleColor(2);*/
			}
			beatIndex += beatChunk;
			beats -= beatChunk;
			ImGui::EndTable();
		}
	}

	//Draw Time Bar
	if(_currentTime != 0)
	{
		auto* window = ImGui::GetCurrentWindow();

		float x = window->Pos.x + (timeline.beats.size()*(noteSize.x + 1)) * (_currentTime / timeline.duration());
		float y = noteSize.y * _allNotes.size();
		window->DrawList->AddLine({x, 0}, {x, y}, ImColor{0.30f, 0.70f, 0.40f, 1.0f});
	}

	ImGui::PopStyleVar(2);
	ImGui::EndChild();
	ImGui::EndChild();
}

void TimelineEditor::loadProject(const std::filesystem::path& path)
{
	if(path.extension() != ".synth")
		return;

	std::ifstream f(path, std::ios::binary);

	uint32_t jsonSize = 0;
	f.read((char*)&jsonSize, sizeof(uint32_t));
	std::string jsonString;
	jsonString.resize(jsonSize);
	f.read(jsonString.data(), jsonSize);

	uint32_t binaryDataSize;
	f.read((char*)&binaryDataSize, sizeof(uint32_t));
	SerializedData binaryData;
	binaryData.vector().resize(binaryDataSize);
	f.read((char*)binaryData.vector().data(), binaryDataSize);
	f.close();
	InputSerializer s(binaryData);

	Json::Value json;
	std::stringstream(jsonString) >> json;

	_synths.resize(0);
	_timelines.resize(0);

	for(auto& synth : json["synths"])
	{
		SynthCtx ctx;
		ctx.name = synth["name"].asString();
		std::unique_ptr<Synth> synthSource;
		if(synth["source"] == "sin")
			synthSource = std::make_unique<SinSynth>();
		if(synth["source"] == "square")
			synthSource = std::make_unique<SquareSynth>();
		if(synth["source"] == "saw")
			synthSource = std::make_unique<SawSynth>();
		ctx.source = std::move(synthSource);
		_synths.push_back(std::move(ctx));
	}

	for(auto& timeline : json["timelines"])
	{
		TimelineCtx ctx;
		ctx.name = timeline["name"].asString();
		ctx.synth = timeline["synth"].asInt();
		ctx.volume = timeline["volume"].asFloat();
		ctx.timeline.bpm = timeline["bpm"].asFloat();

		ctx.timeline.synth = _synths[ctx.synth].source.get();

		auto& beats = ctx.timeline.beats;
		uint32_t beatCount;
		s >> beatCount;
		beats.resize(beatCount);
		for(auto& beat : beats)
		{
			uint32_t notes;
			s >> notes;
			for(uint32_t n = 0; n < notes; ++n)
			{
				float note;
				s >> note;
				beat.notes.insert(note);
			}
		}
		_timelines.push_back(std::move(ctx));
	}
	if(!_timelines.empty())
		_selectedTimeline = 0;
	else
		_selectedTimeline = -1;

    _saveFile = path;
}

void TimelineEditor::saveProject()
{
	if(_saveFile.empty())
	{
		const char* filter[] = {"*.synth"};
		const char* promptRes = tinyfd_saveFileDialog("Save as", std::filesystem::current_path().replace_filename("new project").string().c_str(), 1, filter, "Simple Synth");
		if(!promptRes)
			return;
		_saveFile = promptRes;
	}

	Json::Value json;
	SerializedData binaryData;
	OutputSerializer s(binaryData);

	for(auto& ctx : _synths)
	{
		Json::Value synth;
		synth["name"] = ctx.name;
		std::string synthSource = "none";
		if(dynamic_cast<SinSynth*>(ctx.source.get()))
			synthSource = "sin";
		else if(dynamic_cast<SquareSynth*>(ctx.source.get()))
			synthSource = "square";
		else if(dynamic_cast<SawSynth*>(ctx.source.get()))
			synthSource = "saw";
		synth["source"] = synthSource;
		json["synths"].append(synth);
	}

	for(auto& ctx : _timelines)
	{
		Json::Value timeline;
		timeline["name"] = ctx.name;
		timeline["synth"] = ctx.synth;
		timeline["volume"] = ctx.volume;
		timeline["bpm"] = ctx.timeline.bpm;
		json["timelines"].append(timeline);

		auto& beats = ctx.timeline.beats;
		s << static_cast<uint32_t>(beats.size());
		for(auto& beat : beats)
		{
			s << static_cast<uint32_t>(beat.notes.size());
			for(float note : beat.notes)
				s << note;
		}
	}

	std::ofstream f(_saveFile, std::ios::binary);

	Json::FastWriter writer;
	std::string jsonString = writer.write(json);
	uint32_t jsonSize = jsonString.size();
	f.write((char*)&jsonSize, sizeof(uint32_t));
	f.write(jsonString.data(), jsonSize);

	uint32_t binaryDataSize = binaryData.size();
	f.write((char*)&binaryDataSize, sizeof(uint32_t));
	f.write((char*)binaryData.data(), binaryDataSize);
	f.close();
}




