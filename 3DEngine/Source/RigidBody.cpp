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
		physics_body = collider_comp->physics_body;
		collider_comp->SetRigidBodyComp(this);
		physics_body->SetDynamic(true);
		dynamic = true;
	}
	else
	{
		physics_body = App->physics->GetNewRigidBody(0);
		//Make sure is dynamic
		physics_body->SetDynamic(true);
		dynamic = true;
	}

}

RigidBody::~RigidBody()
{
	if(physics_body)
		delete physics_body;
}

void RigidBody::Update()
{
	if (transform && physics_body && App->clock.delta_time > 0)
	{
		physx::PxVec3 pos;
		physx::PxQuat rot;
		physics_body->GetTransform(pos, rot);
		physics_body->SetDynamic(true);
		transform->SetTransform(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w));
		own_update = true;
	}
	else
		own_update = false;
}

void RigidBody::UpdateTransform()
{
	if(physics_body && !own_update) {
		float3 fpos = transform->GetPosition();
		Quat quat = transform->GetRotQuat();
		physics_body->px_body->setGlobalPose(physx::PxTransform(physx::PxVec3(fpos.x, fpos.y, fpos.z),physx::PxQuat(quat.x,quat.y,quat.z,quat.w)));
	}
}

void RigidBody::ChangeParent(GameObject * new_parent)
{
	Component::ChangeParent(new_parent);
	transform = new_parent->GetTransform();

	collider_comp = LookForCollider();

	if (collider_comp != nullptr)
	{
		collider_comp->rigid_body_comp = this;
		if (physics_body)
			delete physics_body;
		physics_body = collider_comp->physics_body;
		physics_body->SetDynamic(true);
		dynamic = true;
	}
	
	UpdateTransform();
}

void RigidBody::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("RigidBody"))
	{
		if (ImGui::Checkbox("Dynamic", &dynamic)) physics_body->SetDynamic(dynamic);
	}
	ImGui::PopID();
}

void RigidBody::SetRigidBody(const jpPhysicsRigidBody * new_body)
{
	if (physics_body)
		delete physics_body;

	if (new_body) {
		physics_body = (jpPhysicsRigidBody*)new_body;

		if (transform == nullptr && parent)
			transform = parent->GetTransform();

		physx::PxVec3 pos;
		physx::PxQuat rot;
		physics_body->GetTransform(pos, rot);
		transform->SetTransform(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.z, rot.w));
		own_update = true;
	}
	else physics_body = nullptr;

	if (collider_comp) {
		collider_comp->physics_body = physics_body;
		collider_comp->rigid_body_comp = this;
	}
}

void RigidBody::SetColliderComp(RbCollider * new_collider)
{
	collider_comp = new_collider;
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
