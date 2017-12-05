#include "PanelProperties.h"
#include "Application.h"
#include "ModuleWindow.h"

PanelProperties::PanelProperties()
{
	name = "Properties";
	active = true;
}

PanelProperties::~PanelProperties()
{
}

void PanelProperties::Draw()
{
	ImGui::Begin("Properties", &active, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2((float)App->window->width - 250, 19.f), 0);
	ImGui::SetWindowSize(ImVec2(250.f, (float)App->window->height * 2 / 5), 0);

	App->scene_intro->DrawProperties();

	ImGui::End();
}