#include "Application.h"
#include "ModuleResources.h"
#include "Importer.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "ResourceTexture.h"

#include <string>

ModuleResources::ModuleResources(Application* app, bool start_enabled) : Module(app, "Resources", start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
	jope_importer = new Importer();
	return true;
}

bool ModuleResources::CleanUp()
{
	delete jope_importer;
	return true;
}

void ModuleResources::HandleDropEvent(SDL_DropEvent drop_event)
{
	std::string temp;
	jope_importer->Import(drop_event.file, temp);
}

int ModuleResources::Find(const char * file_in_assets) const
{
	//TODO: to method
	for (std::map<int, Resource*>::const_iterator it = resources_map.begin(); it != resources_map.end(); it++)
	{
		
	}

	return 0;
}

int ModuleResources::GenerateNewUID()
{
	LCG UUIDGen;
	return UUIDGen.Int();
}

Resource * ModuleResources::CreateNewResource(RESOURCE_TYPE type, int force_uid)
{
	Resource* ret = nullptr;
	int new_UID = 0;
	if (force_uid == 0)
		new_UID = GenerateNewUID();
	else
		new_UID = force_uid;

	switch (type)
	{
	case RESOURCE_UNKNOW:
		break;
	case RESOURCE_MESH:
		ret = new ResourceMesh(new_UID);
		break;
	case RESOURCE_TEXTURE:
		break;
	case RESOURCE_SCENE:
		break;
	default:
		break;
	}

	std::pair<int, Resource*> resource_pair;
	resource_pair.first = ret->GetUID();
	resource_pair.second = ret;
	
	resources_map.insert(resource_pair);

	return ret;
}
