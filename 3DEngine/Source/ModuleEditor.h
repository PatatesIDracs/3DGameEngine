#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"
#include <queue>

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
	bool	showrecord   	= false;
	bool	recording		= false;

	const std::vector<Profiler*>* app_profiler = nullptr;
};

#endif // !__MODULE_EDITOR_H__
