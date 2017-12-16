#include "DistanceJoint.h"
#include "RigidBody.h"
#include "jpPhysicsRigidBody.h"

#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics.h"

DistanceJoint::DistanceJoint(GameObject * parent, bool isactive) : Component(parent, COMP_DISTANCEJOINT, isactive)
{
	pb1_go_id = -1;
	if (parent != nullptr)
	{
		parent->AddComponent(this);
		((RigidBody*)parent->FindFirstComponent(COMP_TYPE::COMP_RIGIDBODY))->joint_ptr = this;
	}
}

DistanceJoint::~DistanceJoint()
{
	if (distance_joint != nullptr)
		distance_joint->release();
	if (physics_body1 != nullptr)
		physics_body1->joint_ptr = nullptr;
}

void DistanceJoint::Update()
{
	if (listening_to_scene)
	{
		if (!App->scene_intro->ImSelected(parent->GetUUID()))
		{
			if (App->scene_intro->GetSelectedGameObject() == nullptr)
			{
				listening_to_scene = false;
				return;
			}

			physics_body1 = (RigidBody*)App->scene_intro->GetSelectedGameObject()->FindFirstComponent(COMP_TYPE::COMP_RIGIDBODY);
			//A rigid body can only have on joint right now to avoid possible bugs
			if (physics_body1 == nullptr || physics_body1->joint_ptr != nullptr)
			{
				LOGC("Unable to create a joint with this GameObject");
				listening_to_scene = false;
				return;
			}

			listening_to_scene = false;
			CreateJoint();
		}
	}
}

void DistanceJoint::Save(const char * buffer_data, char * cursor, int & bytes_copied)
{
	//identifier and type
	int identifier = COMPONENTIDENTIFIER;
	uint bytes_to_copy = sizeof(identifier);
	memcpy(cursor, &identifier, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(type);
	memcpy(cursor, &type, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//UUID and parent UUID
	bytes_to_copy = sizeof(UUID);
	memcpy(cursor, &UUID, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(parent_UUID);
	memcpy(cursor, &parent_UUID, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//active and unique
	bytes_to_copy = sizeof(bool);
	memcpy(cursor, &active, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &unique, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	///DistanceJoint
	//Set the id to the current physic object here
	//Since we only use the id for save an load we don't need to set it anywhere else on the code
	pb1_go_id = physics_body1->GetUUID();
	bytes_to_copy = sizeof(int);
	memcpy(cursor, &pb1_go_id, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = sizeof(float);
	memcpy(cursor, &min_dist, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &max_dist, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void DistanceJoint::Load(char * cursor, int & bytes_copied)
{
	//UUID and parentUUID
	uint bytes_to_copy = sizeof(int);
	memcpy(&UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&parent_UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//active and unique
	bytes_to_copy = sizeof(bool);
	memcpy(&active, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&unique, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;


	///Distance joint comp
	bytes_to_copy = sizeof(int);
	memcpy(&pb1_go_id, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = sizeof(float);
	memcpy(&min_dist, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&max_dist, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void DistanceJoint::OnLoad(std::vector<GameObject*> gameobjects, std::vector<Component*> components)
{
	if (pb1_go_id != -1)
	{
		for (uint i = 0; i < components.size(); i++)
		{
			if (components[i]->GetUUID() == pb1_go_id)
			{
				physics_body1 = (RigidBody*)components[i];
				physics_body1->joint_ptr = this;
				CreateJoint();
				return;
			}
		}
		pb1_go_id = -1;
	}
}

void DistanceJoint::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int);
	buffer_size += sizeof(float)*2;
}

void DistanceJoint::ChangeParent(GameObject * new_parent)
{
	Component::ChangeParent(new_parent);
	((RigidBody*)parent->FindFirstComponent(COMP_TYPE::COMP_RIGIDBODY))->joint_ptr = this;
}

void DistanceJoint::CreateJoint()
{
	physx::PxVec3 offset = physx::PxVec3(0, 0, 0);
	distance_joint = physx::PxDistanceJointCreate(*App->physics->mPhysics, ((RigidBody*)parent->FindFirstComponent(COMP_TYPE::COMP_RIGIDBODY))->GetPhysicsBody()->GetPxBody(),
		physx::PxTransform(offset), physics_body1->GetPhysicsBody()->GetPxBody(), physx::PxTransform(-offset));
	
	if (distance_joint != nullptr)
	{
		CheckDistaces();
		distance_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
		distance_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);	}
	physics_body1->joint_ptr = this;
}

void DistanceJoint::CheckDistaces()
{
	distance_joint->setMinDistance(min_dist);
	distance_joint->setMaxDistance(max_dist);
}

void DistanceJoint::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("Distance joint"))
	{
		if (physics_body1 != nullptr)
			pb1_name = physics_body1->GetParent()->name;

		ImGui::InputText("", (char*)pb1_name.c_str(), pb1_name.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		
		if(ImGui::Button("Select target")) listening_to_scene = !listening_to_scene;

		if( ImGui::InputFloat("Min distance", (float*)&min_dist, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue)) 		CheckDistaces();
		if (ImGui::InputFloat("Max distance", (float*)&max_dist, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))		CheckDistaces();

	}
	ImGui::PopID();
}

void DistanceJoint::StopUsing(int comp_id)
{
	if (physics_body1->GetUUID() == comp_id)
	{
		distance_joint->release();
		distance_joint = nullptr;
		physics_body1->joint_ptr = nullptr;
		physics_body1 = nullptr;
	}
}
