#ifndef __MODULEPHYSICS_H__
#define __MODULEPHYSICS_H__

#include "Module.h" 
#include "Camera.h"

namespace physx {
	class PxPhysics;
}

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json &config);
	void SaveModuleConfig(Config_Json &config);

	void DrawConfig();

public:
	physx::PxPhysics* mPhysics = nullptr;

private:


};

#endif // !__MODULEPHYSICS_H__