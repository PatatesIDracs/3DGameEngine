#include "PanelProfiler.h"
#include "Application.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Profiler.h"

PanelProfiler::PanelProfiler()
{
	name = "Profiler";
	active = false;

	app_profiler = App->GetProfilerVect();
}

PanelProfiler::~PanelProfiler()
{
}

void PanelProfiler::Draw()
{
	ImGui::Begin("Profiler Test", &active, ImGuiWindowFlags_NoResize);
	{
		if (ImGui::Button("Start Profiler")) {
			if (recordpaused)
			{
				recordpaused = false;
				App->DoRecord();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause Profiler"))
		{
			if (!recordpaused) App->DoRecord();
			recordpaused = true;
		}

		//Show Ms from functions Real Time
		ImGui::Separator();

		ImGui::Text("Modules: ");
		const std::list<Module*>* app = App->GetModulesList();
		std::list<Module*>::const_iterator app_modules = app->begin();

		int count = 0;
		for (; app_modules != app->end(); app_modules++)
		{
			ImGui::RadioButton(app_modules._Ptr->_Myval->GetName(), (int*)&current_module, count);
			count++;
		}
		ImGui::Separator();

		std::vector<char*>* fnames = app_profiler->begin()[current_module]->GetFunctionNames();
		std::vector<float>* item = nullptr;

		for (uint i = 0; i < fnames->size(); i++)
		{
			item = app_profiler->begin()[current_module]->GetFunctionTimeline(fnames->at(i));
			ImGui::PlotHistogram(fnames->at(i), &item->at(0), 60, 0, NULL, 0.0f, 5.0f, ImVec2(0, 80));
		}

	}
	ImGui::End();

	// When Closing DrawProfilerWindow Stops Profiler
	if (!active && !recordpaused)
	{
		recordpaused = !recordpaused;
		App->DoRecord();
	}

}
