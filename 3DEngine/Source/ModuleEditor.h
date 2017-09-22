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
	update_status PostUpdate(float dt);

	bool CleanUp();



private:
	bool	showtestwindow = false;

};

#endif // !__MODULE_EDITOR_H__
