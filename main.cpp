#include "src/graphics/window.h"
#include <imgui/imgui.h>
#include "src/synth/simpleSynth.h"
#include "src/synth/sinSynth.h"
#include "src/synth/compressorSource.h"
#include "src/synth/timelineSource.h"
#include "src/editor/timelineEditor.h"

int main()
{
    Window w;
	SimpleSynth s;

	TimelineEditor editor(&s.device());

	w.run([&](int width, int height){
		ImGui::SetNextWindowPos({0,0});
		ImGui::SetNextWindowSize(ImVec2(width, height));
		editor.draw();
	});
	return 0;
}
