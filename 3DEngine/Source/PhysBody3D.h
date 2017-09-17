#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include <list>

class btRigidBody;
class Module;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();
	bool Coin = false;

	void Push(float x, float y, float z);
	void flip(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void SetAsSensor(bool is_sensor);
	bool IsSensor() const;
	
	btRigidBody* GetBody();

private:
	btRigidBody* body = nullptr;
	bool is_sensor = false;
	
public:
	std::list<Module*> collision_listeners;
};

#endif // __PhysBody3D_H__