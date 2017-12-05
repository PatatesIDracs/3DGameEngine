#include "PanelSaveScene.h"
#include "Application.h"

PanelSaveScene::PanelSaveScene()
{
	name = "SaveScene";
	active = false;
}

PanelSaveScene::~PanelSaveScene()
{
}

void PanelSaveScene::Draw()
{
	ImGui::OpenPopup("Save File");
	if (ImGui::BeginPopupModal("Save File", &active))
	{
		static char file_name[200];
		ImGui::BeginChild("test", ImVec2(300, 300), true);

		ImGui::EndChild();

		ImGui::InputText("", file_name, (int)sizeof(file_name));
		ImGui::SameLine();

		if (ImGui::Button("Save"))
		{
			App->SaveScene(file_name);
			active = false;
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			active = false;

		ImGui::EndPopup();
	}
}