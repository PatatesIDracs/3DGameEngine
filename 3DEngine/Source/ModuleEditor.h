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

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);

	void Draw();

	bool CleanUp();

	void LogToConsole(std::string* log_string);

	void LoadHardwareSoftwareInfo();

	//Return true when a file is selected
	bool DrawFixedExplorer(std::string& output, const char* path);
	bool DrawExplorer(std::string* output_file);

private:

	// Draw Configuration ------------------
	void ApplicationConfig();
	void HardwareDetection();
	
	void DrawPlayButton();
	void DrawClock();
	void DrawConsole();
	void DrawAssets();
	void DrawPropertiesWindow();
	void DrawAboutWindow();
	void DrawSaveWindow();
	void DrawLoadWindow();

	void DrawHierarchy();

	// Draw Profiler -----------------------
	void DrawProfilerWindow();


private:
	//Panel List
	std::vector<Panel*> panel_array;

	bool	showtestwindow			= false;
	bool	showaboutwindow			= false;
	bool	showconfig				= false;
	bool	showconsole				= false;
	bool	showpropertieswindow	= false;
	bool	showhierarchy			= false;
	bool	showclock				= false;
	bool	savewindow				= false;
	bool	loadwindow				= false;
	bool	showassets				= true;

	bool	on_assets				= true;


	// Profiler
	bool	showprofiler			= false;
	bool	recordpaused			= true;

	int		current_module  = 0;
	const std::vector<Profiler*>* app_profiler = nullptr;

	std::vector<std::string>	console_string;

	//App names
	char str_name[512];
	char str_org[512];
	bool uncapped_fps = true;

	//Hardware and software info (so we don't have to use the functions every update)
	//Assimp version
	int assimp_major;
	int assimp_minor;
	int assimp_revision;

	//Devil version
	int devil_version = 0;

	//CPU info
	int		cpu_cores = 0;
	int		cpu_cache_size = 0;
	float	ram = 0;
	bool	has_3Dnow = false;
	bool	has_AVX = false;
	bool	has_AVX2 = false;
	bool	has_AltiVec = false;
	bool	has_MMX = false;
	bool	has_RDTSC = false;
	bool	has_SSE = false;
	bool	has_SSE2 = false;
	bool	has_SSE3 = false;
	bool	has_SSE41 = false;
	bool	has_SSE42 = false;


	//Explorer
	std::string it_library_path;
	std::string it_assets_path;
};

#endif // !__MODULE_EDITOR_H__
