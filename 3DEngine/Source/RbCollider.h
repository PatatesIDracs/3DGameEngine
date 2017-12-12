#ifndef __RB_COLLIDER_H__
#define __RB_COLLIDER_H__

#include "Component.h"

class Transform;
class RigidBody;
class jpPhysicsRigidBody;

enum PHYSCOLL_TYPE
{
	COLL_BOX,
	COLL_SPHERE,

};

class RbCollider : public Component
{
public:

	RbCollider(GameObject* parent, bool isactive = true);
	~RbCollider();

	void Update();
	void UpdateTransform();

	void DrawComponent();

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

public:
	Transform* transform = nullptr;
	RigidBody* rigid_body_comp = nullptr;
	jpPhysicsRigidBody* physics_body = nullptr;
};

#endif // !__RB_COLLIDER_H__
