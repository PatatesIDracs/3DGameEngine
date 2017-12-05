#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"
#include <queue>
#include <list>	
#include <string>
#include <filesystem>

#include "SDL\include\SDL.h"

#define CONSOLE_LIMIT 500

class Profiler;
class Panel;
class Resource;
enum RESOURCE_TYPE;

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);

	void Draw();

	bool CleanUp();

	void LogToConsole(std::string* log_string);

	Resource* DrawResourceExplorer(RESOURCE_TYPE type, const char* folder);

	Panel* GetUIPanel(const char* panel_name) const;

private:

	void DrawConsole();
	void DrawAssets();

	void ChangePanelState(const char* panel_name);

private:
	//Panel List
	std::vector<Panel*> panel_array;

	bool	showtestwindow			= false;
	bool	showconsole				= false;
	bool	showassets				= true;

	bool	on_assets				= true;

	std::vector<std::string>	console_string;

	//Explorer
	std::string it_library_path;
	std::string it_assets_path;
};

#endif // !__MODULE_EDITOR_H__
