#include "jpPhysicsWorld.h"
//#include "Globals.h"
#include "Math.h"

#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3ExtensionsDEBUG.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3CommonDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PxPvdSDKDEBUG_x86.lib")

jpPhysicsWorld::jpPhysicsWorld()
{	
}

jpPhysicsWorld::~jpPhysicsWorld()
{
	if (jpWorld) {
		physx::PxScene* scene = nullptr;
		for (unsigned int i = 0; i < jpWorld->getNbScenes(); i++) {
			jpWorld->getScenes(&scene, 1, i);
			scene->release();
			scene = nullptr;
		}
		jpWorld->release();	
		//pvd->release();
		jpFoundation->release();
	}
}

bool jpPhysicsWorld::CreateNewPhysicsWorld()
{
	if (!jpWorld) {
		jpFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback,
			gDefaultErrorCallback);

		if (jpFoundation)
		{
			//pvd = physx::PxCreatePvd(*jpFoundation);
			//physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			//pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			jpWorld = PxCreatePhysics(PX_PHYSICS_VERSION, *jpFoundation, physx::PxTolerancesScale(), true, nullptr);
		}

		if (jpWorld) {
			//jpCooking = PxCreateCooking(PX_PHYSICS_VERSION, *jpFoundation, jpWorld->getTolerancesScale());
			return true;
		}
		else return false;
	}
	else return false;
}

bool jpPhysicsWorld::Simulate(float dt)
{
	if (jpWorld && jpWorld->getNbScenes() > 0) {
		physx::PxScene* scene;
		jpWorld->getScenes(&scene, 1, 0);
		scene->simulate(dt);
		return scene->fetchResults(true);
	}
	return false;
}

// Create Default Scene
physx::PxScene * jpPhysicsWorld::CreateNewScene()
{
	physx::PxSceneDesc sceneDesc(jpWorld->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	return jpWorld->createScene(sceneDesc);
}

physx::PxScene * jpPhysicsWorld::GetScene(int scene_index) const
{
	physx::PxScene* ret = nullptr;
	jpWorld->getScenes(&ret, 1, scene_index);
	return ret;
}

physx::PxPhysics * jpPhysicsWorld::GetPhysicsWorld()
{
	return jpWorld;
}

physx::PxCooking * jpPhysicsWorld::GetCooking()
{
	return nullptr;
	//return jpCooking;
}

jpPhysicsRigidBody * jpPhysicsWorld::CreateRigidBody(physx::PxScene * curr_scene)
{
	jpPhysicsRigidBody* new_body = nullptr;
	if (jpWorld) {
		new_body = new jpPhysicsRigidBody(jpWorld);
		if (curr_scene != nullptr)
			curr_scene->addActor(*new_body->px_body);
	}
	
	return new_body;
}
