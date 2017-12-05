#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleWindow.h"

PanelHierarchy::PanelHierarchy()
{
	name = "Hierarchy";
	active = true;
}

PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	ImGui::Begin("Game Object hierarchy", &active);
	ImGui::SetWindowPos(ImVec2(0.f, 19.f), 0);
	ImGui::SetWindowSize(ImVec2((float)250, (float)App->window->height - 19), 0);

	App->scene_intro->DrawRootHierarchy();

	ImGui::End();
}