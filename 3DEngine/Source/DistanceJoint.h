#ifndef __DISTANCEJOINT_H__
#define __DISTANCEJOINT_H__

#include "Component.h"
#include <string>

#include "PhysX\Include\PxPhysicsAPI.h"

class RigidBody;

class DistanceJoint : public Component
{
public:
	DistanceJoint(GameObject* parent, bool isactive = true);
	~DistanceJoint();

	void Update();
	void DrawComponent();

	void StopUsing(int comp_id);

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void OnLoad(std::vector<GameObject*> gameobjects, std::vector<Component*> components);
	void GetOwnBufferSize(uint& buffer_size);

	void ChangeParent(GameObject* new_parent);

private:
	void CreateJoint();

	void CheckDistaces();

private:
	bool listening_to_scene = false;

	//Joint bodies
	//Components must always be linked to gameObjects with a RigidBody component
	//parent acts as physics_body0
	RigidBody* physics_body1 = nullptr;
	int pb1_go_id = -1;
	std::string pb1_name;

	//Joint data
	physx::PxDistanceJoint* distance_joint = nullptr;
	float min_dist = 2.0f;
	float max_dist = 10.0f;
};
#endif // !__DISTANCEJOINT_H__

