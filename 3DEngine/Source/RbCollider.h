#ifndef __RB_COLLIDER_H__
#define __RB_COLLIDER_H__

#include "Component.h"

class Transform;
class RigidBody;
class jpPhysicsRigidBody;

enum PHYSCOLL_TYPE
{
	COLL_SPHERE,
	COLL_PLANE,
	COLL_BOX,
	COLL_CAPSULE
};

class RbCollider : public Component
{
public:

	RbCollider(GameObject* parent, bool isactive = true);
	~RbCollider();

	void Update();
	void UpdateTransform();

	void DrawComponent();

	void SetRigidBodyComp(RigidBody* new_rigid_body);


	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

	void ChangeParent(GameObject* new_parent);

	void ChangeCollider();
	void UpdateCollider();

private:
	RigidBody* LookForRigidBody();

public:
	Transform* transform = nullptr;
	RigidBody* rigid_body_comp = nullptr;
	jpPhysicsRigidBody* physics_body = nullptr;

private:
	// Collider_type defines which shape to use
	PHYSCOLL_TYPE		collider_type = COLL_SPHERE;
	PHYSCOLL_TYPE		curr_type = COLL_SPHERE;

	// Collider Data
	float3				material = float3(0.5f, 0.5f, 0.5f);
	float3				position = float3(0.f, 0.f, 0.f);
	float3				size = float3(1.f, 1.f, 1.f);
	float				rad = 0.5f;
};

#endif // !__RB_COLLIDER_H__
