#ifndef __MODULERESOURCES_H__
#define __MODULERESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include <map>

class ModuleResources : public Module
{
public:
	ModuleResources(Application* app, bool start_enabled = true);
	~ModuleResources();

	int Find(const char* file_in_assets) const;

	int GenerateNewUID();
	
private:

	std::map<int, Resource*> resources_map;

};


#endif // !1