#include "ModuleResources.h"

ModuleResources::ModuleResources(Application* app, bool start_enabled) : Module(app, "Resources", start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

int ModuleResources::Find(const char * file_in_assets) const
{

	for (std::map<int, Resource*>::const_iterator it = resources_map.begin(); it != resources_map.end(); it++)
	{
		
	}

	return 0;
}
