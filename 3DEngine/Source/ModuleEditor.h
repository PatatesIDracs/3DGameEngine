#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"
#include <queue>

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);

	void DrawImGui();

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
	bool	showprofiler	= false;

};

#endif // !__MODULE_EDITOR_H__
