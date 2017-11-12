#ifndef __MODULERESOURCES_H__
#define __MODULERESOURCES_H__

#include "Module.h"
#include <map>

class Importer;
class Resource;
enum RESOURCE_TYPE;
struct SDL_DropEvent;

class ModuleResources : public Module
{
public:
	ModuleResources(Application* app, bool start_enabled = true);
	~ModuleResources();

	bool Init();
	bool CleanUp();

	void HandleDropEvent(SDL_DropEvent drop_event);

	int Find(const char* file_in_assets) const;

	int GenerateNewUID();
	Resource* GetFromUID(int uid) const;
	Resource* CreateNewResource(RESOURCE_TYPE type, int force_uid = 0);
	
	const Importer* GetImporter()const;

private:

	std::map<int, Resource*> resources_map;

	Importer* jope_importer;

};


#endif // !1