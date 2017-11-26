#include "PanelAbout.h"

PanelAbout::PanelAbout()
{
	name = "About";
}

PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	ImGui::Begin("About JoPe", &active, ImGuiWindowFlags_NoResize);

	ImGui::Text("JoPe Engine");
	ImGui::Text("Very simple and limited game engine,\nmade for educational purposes");

	ImGui::Text("\nMade by: Joan Pareja, Pere Rifa");

	//Libraries: bullet/PhysX, SDL, glew, glut, dear imgui, json(Parson), MathGeoLib
	ImGui::Text("\nMade with: SDL, glew, dear imgui, Parson, MathGeoLib");

	ImGui::Text("\nLicensed under the MIT license");

	ImGui::End();
}