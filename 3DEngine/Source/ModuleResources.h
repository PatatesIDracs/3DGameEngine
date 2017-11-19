#ifndef __MODULERESOURCES_H__
#define __MODULERESOURCES_H__

#include "Module.h"
#include <map>
#include <vector>

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
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void SearchForResources();						//Look at library for new resources
	void LoadFromAssets();
	void UpdateAssetsFiles();
	bool CheckMetaFiles(std::string& file_path, const char* extension);
	void HandleDropEvent(SDL_DropEvent drop_event);	
	void GotFocus(bool focus_app);

	int GenerateNewUID();
	Resource* GetFromUID(int uid) const;
	Resource* CreateNewResource(RESOURCE_TYPE type, int force_uid = 0);
	void DeleteFileFromUID(int uid);
	
	Importer* GetImporter() const;

public:
	std::vector<Resource*> resources_vec;
	std::vector<Resource*> all_resources_vec;

private:

	std::map<int, Resource*> resources_map;
	Importer* jope_importer;

	bool update_assets = false;
};


#endif // !1