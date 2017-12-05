#include "PanelResourceExplorer.h"
#include "Application.h"
#include "ModuleResources.h"

PanelResourceExplorer::PanelResourceExplorer()
{
	name = "ResourceExplorer";
	active = false;
	type_to_show = RESOURCE_TYPE::RESOURCE_UNKNOW;
	to_show = &App->resources->all_resources_vec;
}

PanelResourceExplorer::~PanelResourceExplorer()
{
}

void PanelResourceExplorer::Draw()
{
	ImGui::Begin(("%s", ResourceTypeToString(type_to_show)), &active);
	ImGui::BeginChild("test", ImVec2(350, 350), true);

	for (uint i = 0; i < to_show->size(); i++)
	{
		if (ImGui::Button(("%s", (*to_show)[i]->GetAssetsPath())))
		{
			to_load = (*to_show)[i];
		}
	}
	ImGui::EndChild();
	ImGui::End();
}

void PanelResourceExplorer::ChangeState()
{
	Panel::ChangeState();
	LoadResourcesArray();
}

void PanelResourceExplorer::SetTypeToShow(RESOURCE_TYPE new_type_to_show)
{
	type_to_show = new_type_to_show;
	LoadResourcesArray();
}

void PanelResourceExplorer::LoadResourcesArray()
{
	switch (type_to_show)
	{
	case RESOURCE_UNKNOW:
		to_show = &App->resources->all_resources_vec;
		break;
	case RESOURCE_MESH:
		//TODO: Get all mesh resources
		break;
	case RESOURCE_TEXTURE:
		//TODO: Get all texture resources
		break;
	case RESOURCE_SCENE:
		//TODO: Get all scene resources
		break;
	default:
		break;
	}
}

Resource * PanelResourceExplorer::GetLoadedResource() const
{
	return to_load;
}

const char * PanelResourceExplorer::ResourceTypeToString(RESOURCE_TYPE type)
{
	const char* ret = "";
	switch (type)
	{
	case RESOURCE_UNKNOW:
		ret = "Resources";
		break;
	case RESOURCE_MESH:
		ret = "Meshes";
		break;
	case RESOURCE_TEXTURE:
		ret = "Textures";
		break;
	case RESOURCE_SCENE:
		ret = "Scenes";
		break;
	default:
		break;
	}

	return ret;
}