#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "Component.h"

class Transform;
class jpPhysicsRigidBody;
class RbCollider;
class DistanceJoint;

class RigidBody : public Component
{
public:

	RigidBody(GameObject* parent, bool isactive = true);
	~RigidBody();

	void Update();
	void UpdateTransform();
	void ChangeParent(GameObject* new_parent);

	void DrawComponent();

	// Set Body
	void SetPhysicsBody(jpPhysicsRigidBody* new_physics_body);
	void SetPhysicsBodyMass();
	void SetColliderComp(RbCollider* new_collider);

	jpPhysicsRigidBody* GetPhysicsBody();

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:

	RbCollider* LookForCollider();

public:
	//In case the rigidbody is used for a joint this id will be diferent than -1
	DistanceJoint* joint_ptr = nullptr;

private:
	Transform* transform = nullptr;
	RbCollider* collider_comp = nullptr;
	jpPhysicsRigidBody* physics_body = nullptr;

	bool dynamic = true;
	bool own_update = false;

	float mass = 1.f;
};


#endif // !__RIGIDBODY_H__