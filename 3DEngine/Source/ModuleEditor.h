#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"
#include <queue>
#include <list>	
#include <string>

class Profiler;

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


private:

	// Draw Configuration ------------------
	void DrawAboutWindow();
	void ApplicationConfig();
	void HardwareDetection();
	void DrawConsole();
	
	// Draw Profiler -----------------------
	void DrawProfilerWindow();


private:
	bool	showtestwindow	= false;
	bool	showaboutwindow	= false;
	bool	showconfig		= false;
	bool	showconsole		= false;

	// Profiler
	bool	showprofiler	= false;
	bool	recordpaused	= true;

	int		current_module  = 0;
	const std::vector<Profiler*>* app_profiler = nullptr;

	std::list<std::string>	console_string;
};

#endif // !__MODULE_EDITOR_H__
