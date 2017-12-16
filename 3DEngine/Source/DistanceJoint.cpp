#include "DistanceJoint.h"
#include "RigidBody.h"
#include "jpPhysicsRigidBody.h"

#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics.h"

DistanceJoint::DistanceJoint(GameObject * parent, bool isactive) : Component(parent, COMP_DISTANCEJOINT, isactive)
{
	if (parent != nullptr)
	{
		parent->AddComponent(this);
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
			//A rigid body can only have on joint right now
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
