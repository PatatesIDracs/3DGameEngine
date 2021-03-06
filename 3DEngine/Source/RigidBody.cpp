#include "RigidBody.h"
#include "RbCollider.h"
#include "DistanceJoint.h"

#include "Application.h"
#include "Transform.h"
#include "jpPhysicsRigidBody.h"

RigidBody::RigidBody(GameObject * parent, bool isactive) : Component(parent, COMP_RIGIDBODY, true)
{
	joint_ptr = nullptr;
	if (parent != nullptr)
		parent->AddComponent(this);
}

RigidBody::~RigidBody()
{
	if (physics_body) {
		delete physics_body;
		physics_body = nullptr;
	}

	if (collider_comp != nullptr) {
		collider_comp->SetRigidBodyComp(nullptr);
		collider_comp->SetPhysicsBody(nullptr);
	}

	if (joint_ptr != nullptr && joint_ptr->GetParentUUID() != parent_UUID)//UUID cheked since we don't want to call it if its a sibling component
	{
		joint_ptr->StopUsing(UUID);
	}

}

void RigidBody::Update()
{
	if (transform && physics_body && App->clock.delta_time > 0.f)
	{
		// Get transform from physics
		physx::PxVec3 pos;
		physx::PxQuat rot;
		physics_body->GetTransform(pos, rot);

		// Change variables to mathgeolib
		float3 position = float3(pos.x, pos.y, pos.z);
		Quat rotation = Quat(rot.x, rot.y, rot.z, rot.w);
	
		Quat global_rot = transform->GetParentQuat().Conjugated();

		// Global to local transform
		float3 p_pos = transform->GetParentPos();
		float3 p_scale = transform->GetParentScale();
		rotation = global_rot*rotation;
		position = global_rot*(position - transform->GetParentPos());
		position = float3(position.x / p_scale.x, position.y / p_scale.y, position.z / p_scale.z);

		// Collider to local transform
		if (collider_comp) {
			Quat local_rot = collider_comp->GetLocalQuat().Conjugated();
			position -= rotation*collider_comp->GetPosition();
			rotation = rotation*local_rot;
		}

		// GameObject local transform and position
		transform->SetTransform(position, Quat(rotation.x, rotation.y, rotation.z, rotation.w));
		own_update = true;
	}
	else own_update = false;
}

void RigidBody::UpdateTransform()
{
	if(physics_body && !own_update) {
		Quat quat = transform->GetParentQuat()*transform->CurrRotQuat();
		float3 fpos = transform->GetGlobalPos();
		// Get Local Transform if component have collider_comp assosiated
		if (collider_comp) {
			quat = quat*collider_comp->GetLocalQuat();
			fpos += quat*collider_comp->GetPosition();	
		}
		physics_body->px_body->setGlobalPose(physx::PxTransform(physx::PxVec3(fpos.x, fpos.y, fpos.z), physx::PxQuat(quat.x, quat.y, quat.z, quat.w)));
	}
}

void RigidBody::ChangeParent(GameObject * new_parent)
{
	Component::ChangeParent(new_parent);
	
	collider_comp = LookForCollider();

	if (collider_comp != nullptr)
	{
		collider_comp->SetRigidBodyComp(this);
		if (physics_body)
			delete physics_body;
		physics_body = collider_comp->GetPhysicsBody();
	}
	else {
		physics_body = App->physics->GetNewRigidBody(0);
	}
	//Make sure is dynamic
	physics_body->SetDynamic(dynamic);

	transform = new_parent->GetTransform();
	if (!transform->update_transform)
		transform->update_transform = true;
}

void RigidBody::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("RigidBody"))
	{
		if (ImGui::Checkbox("Dynamic", &dynamic)) physics_body->SetDynamic(dynamic);
		if (ImGui::InputFloat("Mass", (float*)&mass, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			SetPhysicsBodyMass();
	}
	ImGui::PopID();
}

void RigidBody::SetPhysicsBody(jpPhysicsRigidBody * new_physics_body)
{
	// careful somethings wrong here
	physics_body = new_physics_body;
}

void RigidBody::SetPhysicsBodyMass()
{
	if (physics_body)
		physics_body->SetMass(mass);
}

void RigidBody::SetColliderComp(RbCollider * new_collider)
{
	collider_comp = new_collider;
	
	if (new_collider){
		jpPhysicsRigidBody* body = new_collider->GetPhysicsBody();
		if (body && physics_body != body) {
			if (physics_body)
				delete physics_body;
			physics_body = body;
			physics_body->SetDynamic(dynamic);
		}
	}
}

jpPhysicsRigidBody * RigidBody::GetPhysicsBody()
{
	return physics_body;
}

void RigidBody::Save(const char * buffer_data, char * cursor, int & bytes_copied)
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


	///RigidBody comp
	bytes_to_copy = sizeof(bool);
	memcpy(cursor, &dynamic, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &own_update, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = sizeof(float);
	memcpy(cursor, &mass, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void RigidBody::Load(char * cursor, int & bytes_copied)
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


	///RigidBody comp
	bytes_to_copy = sizeof(bool);
	memcpy(&dynamic, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&own_update, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = sizeof(float);
	memcpy(&mass, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void RigidBody::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(bool) * 2;
	buffer_size += sizeof(float);
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
