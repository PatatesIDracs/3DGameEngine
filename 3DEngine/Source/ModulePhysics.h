#ifndef __MODULEPHYSICS_H__
#define __MODULEPHYSICS_H__

#include "Module.h" 

#define	VERY_LITTLE_NUMBER 0.00000000000000000000000000001

class jpPhysicsWorld;
class jpPhysicsRigidBody;
class GameObject;

namespace physx {
	class PxPhysics;
	class PxScene;
	class PxSceneDesc;
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxBounds3;
}

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

	// Set Culling for Debug Draw, uses current camera frustum as limits
	void SetDebugCullingLimits(AABB& cull_box);

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json &config);
	void SaveModuleConfig(Config_Json &config);

	void DrawConfig();

	// Rigid Body Test, shot balls
	void ShotBalls();

	void DrawPhysics();

public:
	physx::PxPhysics* mPhysics = nullptr;
private:

	jpPhysicsWorld* physics_world = nullptr;
	physx::PxScene* mScene = nullptr;
	
	// Shot Balls
	std::vector<GameObject*> shot_balls;
	uint curr_ball = 0;

	//Config data
	bool render_physics = false;
	bool render_on_play = false;
};

#endif // !__MODULEPHYSICS_H__