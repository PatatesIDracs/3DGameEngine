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
}

void DistanceJoint::Update()
{
	if (listening_to_scene)
	{
		if (!App->scene_intro->ImSelected(parent->GetUUID()))
		{
			physics_body1 = App->scene_intro->GetSelectedGameObject();

			RigidBody* rigid_body1 = (RigidBody*)physics_body1->FindFirstComponent(COMP_TYPE::COMP_RIGIDBODY);

			if (rigid_body1 == nullptr)
			{
				physics_body1 = nullptr;
				LOGC("This object doesn't have a rigidbody attached to it");
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
	physx::PxVec3 offset = physx::PxVec3(0, 2, 0);
	distance_joint = physx::PxDistanceJointCreate(*App->physics->mPhysics, ((RigidBody*)parent->FindFirstComponent(COMP_TYPE::COMP_RIGIDBODY))->GetPhysicsBody()->GetPxBody(),
		physx::PxTransform(offset), ((RigidBody*)physics_body1->FindFirstComponent(COMP_TYPE::COMP_RIGIDBODY))->GetPhysicsBody()->GetPxBody(), physx::PxTransform(-offset));

	distance_joint->setMinDistance(min_dist);
	distance_joint->setMaxDistance(max_dist);
	distance_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED,  true);
	distance_joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
}

void DistanceJoint::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("Distance joint"))
	{
		if (physics_body1 != nullptr)
			pb1_name = physics_body1->name;

		ImGui::InputText("", (char*)pb1_name.c_str(), pb1_name.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		
		if(ImGui::Button("Select target")) listening_to_scene = !listening_to_scene;

	}
	ImGui::PopID();
}