#ifndef __MODULE_H__
#define __MODULE_H__


#include "Globals.h"
#include <vector>
#include <list>

#define NAME_LENGTH 30

class Application;
struct PhysBody3D;

class Config_Json;

class Module
{
private :
	bool enabled;
	char name[NAME_LENGTH];
public:
	Application* App;

	Module(Application* parent, const char* module_name = "", bool start_enabled = true) : App(parent)
	{
		strcpy_s(name, NAME_LENGTH, module_name);
	}

	virtual ~Module()
	{}

	const char* GetName() const
	{
		return name;
	}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void DrawImGui()
	{}

	virtual void DrawConfig()
	{}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}

	// JSON Save and Load Configuration
	virtual void LoadModuleConfig(Config_Json&) {};
	virtual void SaveModuleConfig(Config_Json&) {};

};

#endif // !__MODULE_H__