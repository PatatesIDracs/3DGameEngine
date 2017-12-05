#ifndef __PANEL_H__
#define __PANEL_H__

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"
#include <string>

class Panel
{

public:
	Panel();
	virtual ~Panel();

	virtual void Draw();

	//virtual since some panels might want to load data when they are activated
	virtual void ChangeState();
	bool IsActive() const;

	const char* GetName() const;
protected:
	bool active = false;
	std::string name;
};

#endif // !__PANEL_H__
