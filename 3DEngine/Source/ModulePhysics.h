#ifndef __MODULEPHYSICS_H__
#define __MODULEPHYSICS_H__

#include "Module.h" 

class jpPhysicsWorld;
class jpPhysicsRigidBody;
class GameObject;

namespace physx {
	class PxPhysics;
	class PxScene;
	class PxSceneDesc;
	class PxRigidStatic;
	class PxRigidDynamic;
}
class GameObject;

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

	jpPhysicsWorld* physics_world = nullptr;

	GameObject* plane = nullptr;
	GameObject* cube = nullptr;


	physx::PxRigidDynamic* box = nullptr;
	physx::PxRigidStatic* nplane = nullptr;

	physx::PxScene* mScene = nullptr;
};

#endif // !__MODULEPHYSICS_H__