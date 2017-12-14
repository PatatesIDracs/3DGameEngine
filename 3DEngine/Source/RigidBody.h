#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "Component.h"

class Transform;
class jpPhysicsRigidBody;
class RbCollider;

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
	void SetRigidBody(const jpPhysicsRigidBody* new_body = nullptr);
	void SetColliderComp(RbCollider* new_collider);

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:

	RbCollider* LookForCollider();

public:
	Transform* transform = nullptr;
	RbCollider* collider_comp = nullptr;
	jpPhysicsRigidBody* rigid_body = nullptr;

	bool dynamic = true;
	bool own_update = false;
};


#endif // !__RIGIDBODY_H__