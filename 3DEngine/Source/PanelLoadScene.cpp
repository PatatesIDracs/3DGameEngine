#include "PanelLoadScene.h"
#include "Application.h"
#include "Resource.h"


PanelLoadScene::PanelLoadScene()
{
	name = "LoadScene";
	active = false;
}

PanelLoadScene::~PanelLoadScene()
{
}

void PanelLoadScene::Draw()
{
	std::vector<Resource*>* to_show = &App->resources->scene_vec;

	ImGui::OpenPopup("Load File");
	if (ImGui::BeginPopupModal("Load File", &active))
	{
		static int file_uid = -1;
		static std::string file_name;
		ImGui::BeginChild("test", ImVec2(250, 300), true);

		for (uint i = 0; i < to_show->size(); i++)
		{
			if (ImGui::Button(("%s", (*to_show)[i]->GetAssetsPath())))
			{
				file_uid = (*to_show)[i]->GetUID();
				file_name = (*to_show)[i]->GetAssetsPath();
			};
		}

		ImGui::EndChild();

		ImGui::InputText("", (char*)file_name.c_str(), file_name.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			if (file_uid != -1)
				App->LoadScene(file_uid);
			else
				LOGC("Please select a file to load");
			active = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			active = false;
		}
		ImGui::EndPopup();
	}
}
