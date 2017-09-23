#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"

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

	void DrawAboutWindow();
	void HardwareDetection();


private:
	bool	showtestwindow	= false;
	bool	showaboutwindow	= false;
	bool	showhardware	= false;
	bool	showconfig		= false;

};

#endif // !__MODULE_EDITOR_H__
