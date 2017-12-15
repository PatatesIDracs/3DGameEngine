#ifndef __RB_COLLIDER_H__
#define __RB_COLLIDER_H__

#include "Component.h"

class Transform;
class RigidBody;
class jpPhysicsRigidBody;

enum JP_COLLIDER_TYPE;

class RbCollider : public Component
{
public:

	RbCollider(GameObject* parent, bool isactive = true);
	~RbCollider();

	void UpdateTransform();

	void DrawComponent();

	void SetRigidBodyComp(RigidBody* new_rigid_body);
	void SetPhysicsBody(jpPhysicsRigidBody* new_physics_body);

	jpPhysicsRigidBody* GetPhysicsBody();

	void ChangeCollider();
	void UpdateCollider();

	void ChangeParent(GameObject* new_parent);

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:
	RigidBody* LookForRigidBody();

private:
	Transform* transform = nullptr;
	RigidBody* rigid_body_comp = nullptr;
	jpPhysicsRigidBody* physics_body = nullptr;

	// Collider_type defines which shape to use
	JP_COLLIDER_TYPE	collider_type = (JP_COLLIDER_TYPE)0;
	JP_COLLIDER_TYPE	curr_type = (JP_COLLIDER_TYPE)0;

	// Collider Data
	float3				material = float3(0.5f, 0.5f, 0.5f);
	float3				position = float3(0.f, 0.f, 0.f);
	float3				size = float3(1.f, 1.f, 1.f);
	float				rad = 0.5f;
};

#endif // !__RB_COLLIDER_H__
