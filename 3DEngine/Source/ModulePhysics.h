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

	//Pass a -1 as argument to create a RigidBody not linked to anyScene
	jpPhysicsRigidBody* GetNewRigidBody(int scene_to_load_it);

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json &config);
	void SaveModuleConfig(Config_Json &config);

	void DrawConfig();

	// Rigid Body Test, shot balls
	void ShotBalls();

public:
	physx::PxPhysics* mPhysics = nullptr;
private:

	jpPhysicsWorld* physics_world = nullptr;
	physx::PxScene* mScene = nullptr;

	// Shot Balls
	std::vector<GameObject*> shot_balls;
	uint curr_ball = 0;
};

#endif // !__MODULEPHYSICS_H__