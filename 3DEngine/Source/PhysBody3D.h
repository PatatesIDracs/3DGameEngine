#ifndef __PHYSBODY3D_H__
#define __PHYSBODY3D_H__

#include "ModuleSceneIntro.h"

class btRigidBody;
class Module;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D();
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	btRigidBody* GetRigidBody();
	btRigidBody* body = nullptr;
	void SetAsSensor(bool is_sensor);
	bool IsSensor() const;
private:
	bool is_sensor = false;

public:
	std::list<Module*> collision_listeners;
	bool tokill= false;
	bool active=true;
};

#endif // __PHYSBODY3D_H__
