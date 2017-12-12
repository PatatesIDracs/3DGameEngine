#include "RigidBody.h"

#include "Application.h"
#include "Transform.h"
#include "jpPhysicsRigidBody.h"

RigidBody::RigidBody(GameObject * parent, bool isactive) : Component(parent, COMP_RIGIDBODY, true)
{
	if (parent != nullptr)
		transform = parent->GetTransform();
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

void RigidBody::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("RigidBody"))
	{
		
	}
	ImGui::PopID();
}

void RigidBody::SetRigidBody(const jpPhysicsRigidBody * new_body)
{
	if (rigid_body) {

	}
	else {
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
