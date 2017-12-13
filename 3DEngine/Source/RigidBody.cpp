#include "RigidBody.h"
#include "RbCollider.h"

#include "Application.h"
#include "Transform.h"
#include "jpPhysicsRigidBody.h"

RigidBody::RigidBody(GameObject * parent, bool isactive) : Component(parent, COMP_RIGIDBODY, true)
{
	if (parent != nullptr)
		transform = parent->GetTransform();

	collider_comp = LookForCollider();

	if (collider_comp != nullptr)
	{
		rigid_body = collider_comp->physics_body;
		rigid_body->SetDynamic(true);
		dynamic = true;
	}
	else
	{
		rigid_body = App->physics->GetNewRigidBody(0);
		//Make sure is dynamic
		rigid_body->SetDynamic(true);
		dynamic = true;
	}

}

RigidBody::~RigidBody()
{
}

void RigidBody::Update()
{
	if (transform && rigid_body && App->clock.delta_time > 0)
	{
		physx::PxVec3 pos;
		physx::PxQuat rot;
		rigid_body->GetTransform(pos, rot);
		transform->SetTransform(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w));
		own_update = true;
	}
	else
		own_update = false;
}

void RigidBody::UpdateTransform()
{
	if(rigid_body && !own_update) {
		float3 fpos = transform->GetPosition();
		Quat quat = transform->GetRotQuat();
		rigid_body->px_body->setGlobalPose(physx::PxTransform(physx::PxVec3(fpos.x, fpos.y, fpos.z),physx::PxQuat(quat.x,quat.y,quat.z,quat.w)));
	}
}

void RigidBody::ChangeParent(GameObject * new_parent)
{
	Component::ChangeParent(new_parent);
	transform = new_parent->GetTransform();

	collider_comp = LookForCollider();

	if (collider_comp != nullptr)
	{
		rigid_body = collider_comp->physics_body;
		rigid_body->SetDynamic(true);
		dynamic = true;
	}
}

void RigidBody::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("RigidBody"))
	{
		if (ImGui::Checkbox("Dynamic", &dynamic)) rigid_body->SetDynamic(dynamic);
	}
	ImGui::PopID();
}

void RigidBody::SetRigidBody(const jpPhysicsRigidBody * new_body)
{
	if (rigid_body) {

	}
	else 
	{
		rigid_body = (jpPhysicsRigidBody*)new_body;

		if (transform == nullptr && parent)
			transform = parent->GetTransform();

		physx::PxVec3 pos;
		physx::PxQuat rot;
		rigid_body->GetTransform(pos, rot);
		transform->SetTransform(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w));
		own_update = true;	
	}
}

void RigidBody::Save(const char * buffer_data, char * cursor, int & bytes_copied)
{
}

void RigidBody::Load(char * cursor, int & bytes_copied)
{
}

void RigidBody::GetOwnBufferSize(uint & buffer_size)
{
}

RbCollider * RigidBody::LookForCollider()
{
	if (parent != nullptr)
	{
		for (uint i = 0; i < parent->components.size(); i++)
		{
			if (parent->components[i]->GetType() == COMP_TYPE::COMP_RBSHAPE)
			{
				return (RbCollider*)parent->components[i];
			}
		}
	}
	return nullptr;
}
