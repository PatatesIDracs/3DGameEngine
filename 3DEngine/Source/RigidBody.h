#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "Component.h"

class Transform;
class jpPhysicsRigidBody;

class RigidBody : public Component
{
public:

	RigidBody(GameObject* parent, bool isactive = true);
	~RigidBody();

	void Update();
	void UpdateTransform();

	void DrawComponent();

	// Set Body
	void SetRigidBody(const jpPhysicsRigidBody* new_body = nullptr);

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

public:
	Transform* transform = nullptr;

	jpPhysicsRigidBody* rigid_body = nullptr;

	bool own_update = false;
};


#endif // !__RIGIDBODY_H__