#include "PanelConfig.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

#include "MeshImporter.h"
#include "TextureImporter.h"

#include "Fluid_Studios_Memory_Manager\mmgr.h"
#include "Fluid_Studios_Memory_Manager\nommgr.h"

PanelConfig::PanelConfig()
{


	//Load Hardware and software info
	///Dummy importers to get the version number
	TextureImporter dummy_tex;
	MeshImporter dummy_mesh;

	dummy_mesh.GetAssimpVersion(assimp_major, assimp_minor, assimp_revision);
	devil_version = dummy_tex.GetDevilVersion();

	cpu_cores = SDL_GetCPUCount();
	cpu_cache_size = SDL_GetCPUCacheLineSize();
	ram = ((float)SDL_GetSystemRAM() / 1024.0f);
	has_3Dnow = SDL_Has3DNow();
	has_AVX = SDL_HasAVX();
	has_AVX2 = SDL_HasAVX2();
	has_AltiVec = SDL_HasAltiVec();
	has_MMX = SDL_HasMMX();
	has_RDTSC = SDL_HasRDTSC();
	has_SSE = SDL_HasSSE();
	has_SSE2 = SDL_HasSSE2();
	has_SSE3 = SDL_HasSSE3();
	has_SSE41 = SDL_HasSSE41();
	has_SSE42 = SDL_HasSSE42();
}

PanelConfig::~PanelConfig()
{
}

void PanelConfig::Draw() 
{
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2((float)App->window->width - 250, (float)App->window->height * 2 / 5 + 19), 0);
	ImGui::SetWindowSize(ImVec2((float)250, (float)App->window->height * 3 / 5 - 19), 0);

	DrawAppConfig();

	module_list = App->GetModulesList();
	std::list<Module*>::const_iterator item = module_list->begin();
	while (item != module_list->end())
	{
		if (ImGui::CollapsingHeader(item._Ptr->_Myval->GetName()))
		{
			item._Ptr->_Myval->DrawConfig();
		}
		item++;
	}
	DrawHardware();
	ImGui::End();
}

void PanelConfig::DrawAppConfig()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		if (ImGui::InputText("App name", str_name, (int)sizeof(str_name), ImGuiInputTextFlags_EnterReturnsTrue))
			App->window->SetTitle(str_name);

		ImGui::InputText("Organitzation", str_org, (int)sizeof(str_org));

		if (ImGui::Checkbox("Unlimited", &uncapped_fps)) App->SetFpsCap(uncapped_fps);
		if (ImGui::SliderInt("Max FPS", &App->fps, 15, 120)) App->SetFpsCap(uncapped_fps);


		ImGui::PlotHistogram("Framerate", &App->GetFPS()->front(), App->GetFPS()->size(), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
		ImGui::PlotHistogram("Miliseconds", &App->GetMs()->front(), App->GetMs()->size(), 0, NULL, 0.0f, 50.0f, ImVec2(0, 80));

		sMStats mem_stats = m_getMemoryStatistics();
		ImGui::Text("Total Reported Mem: %u", mem_stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: %u", mem_stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: %u", mem_stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: %u", mem_stats.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %u", mem_stats.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %u", mem_stats.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %u", mem_stats.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %u", mem_stats.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %u", mem_stats.peakAllocUnitCount);

	}
}

void PanelConfig::DrawHardware()
{

	if (ImGui::CollapsingHeader("Hardware & software"))
	{
		ImVec4	cyan(0.0f, 1.0f, 1.0f, 1.0f);

		//SDL info--------------
		ImGui::Text("SDL version: %i.%i.%i", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

		//Assimp info-----------
		ImGui::Text("Assimp version: %i.%i.%i", assimp_major, assimp_minor, assimp_revision);

		//Devil info
		ImGui::Text("Devil version: %i", devil_version);

		ImGui::Separator();
		//CPU info--------------
		ImGui::Text("CPU: "); ImGui::SameLine();
		ImGui::TextColored(cyan, "%i cores", SDL_GetCPUCount());  ImGui::SameLine();
		ImGui::TextColored(cyan, "(cache: %i bytes)", SDL_GetCPUCacheLineSize());

		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(cyan, "%0.1f Gb", ((float)SDL_GetSystemRAM() / 1024.0f));

		ImGui::Separator();
		//CPU info--------------
		ImGui::Text("CPU: "); ImGui::SameLine();
		ImGui::TextColored(cyan, "%i cores", SDL_GetCPUCount());  ImGui::SameLine();
		ImGui::TextColored(cyan, "(cache: %i bytes)", SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(cyan, "%0.1f Gb", ((float)SDL_GetSystemRAM() / 1024.0f));

		ImGui::Text("Caps: "); ImGui::SameLine();
		if (has_3Dnow)		ImGui::TextColored(cyan, "3DNow!, ");	ImGui::SameLine();
		if (has_AVX)		ImGui::TextColored(cyan, "AVX, ");		ImGui::SameLine();
		if (has_AVX2)		ImGui::TextColored(cyan, "AVX2, ");		ImGui::SameLine();
		if (has_AltiVec)	ImGui::TextColored(cyan, "AltiVec, ");	ImGui::SameLine();
		if (has_MMX)		ImGui::TextColored(cyan, "MMX, ");		ImGui::SameLine();
		if (has_RDTSC)		ImGui::TextColored(cyan, "RDTSC, ");	ImGui::SameLine();
		if (has_SSE)		ImGui::TextColored(cyan, "SSE, ");		ImGui::SameLine();
		if (has_SSE2)		ImGui::TextColored(cyan, "SSE2, ");		ImGui::SameLine();
		if (has_SSE3)		ImGui::TextColored(cyan, "SSE3, ");		ImGui::SameLine();
		if (has_SSE41)		ImGui::TextColored(cyan, "SSE41, ");	ImGui::SameLine();
		if (has_SSE42)		ImGui::TextColored(cyan, "SSE42, ");	ImGui::Separator();

		//GPU info-------------
		ImGui::Text("Vendor: "); ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VENDOR));

		ImGui::Text("Brand: "); ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_RENDERER));

		ImGui::Text("OpenGL Supported: "); ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VERSION));
	}

}
