//
// Created by eli on 9/9/2022.
//

#ifndef SIMPLESYNTH_TIMELINEEDITOR_H
#define SIMPLESYNTH_TIMELINEEDITOR_H


#include <filesystem>
#include <mutex>
#include "../synth/timelineSource.h"

class TimelineEditorPlayer;
class TimelineSource;
class SoundDevice;
class TimelineEditor
{
	friend class TimelineEditorPlayer;
	TimelineEditorPlayer* _player;

	SoundDevice* _device;
	std::filesystem::path _saveFile;
	std::vector<std::pair<std::string, float>> _allNotes;

	std::mutex _mutex;
	bool _playing = false;
	double _currentTime = 0;

	struct TimelineCtx
	{
		std::string name;
		float volume = 0.6;
		int synth = -1;
		TimelineSource timeline;
	};

	struct SynthCtx
	{
		std::string name;
		std::unique_ptr<Synth> source;
	};

	std::vector<TimelineCtx> _timelines;
	std::vector<SynthCtx> _synths;
	int _selectedTimeline = -1;

	float _noteScrollPos = 0;
	float _noteScrollMax = 0;
	bool _noteScrollSet = false;

	void drawMenu();
	void drawNotePickerWindow();
	void drawNotePicker();
	void loadProject(const std::filesystem::path& path);
	void saveProject();

public:
	TimelineEditor(SoundDevice* device);
	~TimelineEditor();
	void draw();
};


#endif //SIMPLESYNTH_TIMELINEEDITOR_H
