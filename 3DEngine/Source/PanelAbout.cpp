#include "PanelAbout.h"
#include "ModuleResources.h"
#include "Application.h"

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

	//SDL libraries
	ImGui::Text("Engine made using SDL libraries: "); ImGui::SameLine();
	if (ImGui::Button("More info")) App->OpenBrowser("https://www.libsdl.org/");

	//OenGL and glew
	if (ImGui::Button("OpenGL")) App->OpenBrowser("https://www.opengl.org/"); ImGui::SameLine();
	ImGui::Text("with"); ImGui::SameLine();
	if (ImGui::Button("GLEW")) App->OpenBrowser("http://glew.sourceforge.net/"); ImGui::SameLine();
	ImGui::Text("are used for the 3D render.");

	//MathgeoLib
	if (ImGui::Button("MathGeoLib")) App->OpenBrowser("http://clb.demon.fi/MathGeoLib/nightly/"); ImGui::SameLine();
	ImGui::Text("used for logical calculations.");

	//Assimp
	if (ImGui::Button("Assimp")) App->OpenBrowser("http://assimp.sourceforge.net/"); ImGui::SameLine();
	ImGui::Text("used to load .fbx and .obj files.");

	//Devil
	if (ImGui::Button("Devil")) App->OpenBrowser("http://openil.sourceforge.net/"); ImGui::SameLine();
	ImGui::Text("used to load texture files.");

	//Mermory usage (mmgr and Deleaker)
	if (ImGui::Button("MMGR")) App->OpenBrowser("http://www.paulnettle.com/"); ImGui::SameLine(); ImGui::Text("and"); ImGui::SameLine();
	if (ImGui::Button("Deleaker")) App->OpenBrowser("http://www.deleaker.com/"); ImGui::SameLine();
	ImGui::Text("used to control memory managment.");

	//ImGui and ImGuizmo
	if (ImGui::Button("ImGui")) App->OpenBrowser("https://github.com/ocornut/imgui"); ImGui::SameLine();
	ImGui::Text("used to make the UI.");
	if (ImGui::Button("ImGuizmo")) App->OpenBrowser("https://github.com/CedricGuillemet/ImGuizmo"); ImGui::SameLine();
	ImGui::Text("used to create the gizmos.");

	//PhysX
	ImGui::Text("Physics done with NVDIA PhysX");
	ImGui::Image((ImTextureID)App->resources->physX_icon_id, ImVec2(256.f, 128.f), ImVec2(0, 1), ImVec2(1, 0));


	ImGui::Text("\nLicensed under the MIT license");

	ImGui::End();
}