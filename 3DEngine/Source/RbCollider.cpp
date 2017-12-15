#include "RbCollider.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Application.h"
#include "ModulePhysics.h"

#include "Transform.h"

#include "jpPhysicsRigidBody.h"

#include "Glew\include\glew.h"

RbCollider::RbCollider(GameObject * parent, bool isactive) : Component(parent, COMP_RBSHAPE, true)
{
	if (parent != nullptr)
		parent->AddComponent(this);
}

RbCollider::~RbCollider()
{
	if (physics_body && !rigid_body_comp) {
		delete physics_body;
		physics_body = nullptr;
	}
	
	if (rigid_body_comp != nullptr) {
		rigid_body_comp->SetColliderComp(nullptr);
		rigid_body_comp->SetPhysicsBody(nullptr);
	}
}

void RbCollider::UpdateTransform()
{
	if (!rigid_body_comp && physics_body) {
		float3 fpos = transform->GetPosition();
		Quat quat = transform->GetRotQuat();
		physics_body->px_body->setGlobalPose(physx::PxTransform(physx::PxVec3(fpos.x, fpos.y, fpos.z), physx::PxQuat(quat.x, quat.y, quat.z, quat.w)));
	}
}

void RbCollider::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("Rb Collider"))
	{
		// Collider type
		if (ImGui::Combo("Collider", (int*)&collider_type, "Sphere\0Plane\0Capsule\0Box\0", 4) && curr_type != collider_type) {
			curr_type = collider_type;
			ChangeCollider();
		}

		// Collider Position
		if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && physics_body) {
			physx::PxTransform transf =	physics_body->px_body->getGlobalPose();
			transf.p = physx::PxVec3(position.x, position.y, position.z);
			physics_body->px_body->setGlobalPose(transf);
		}

		// Collider
		switch (collider_type)
		{
		case COLL_BOX:
			if (ImGui::InputFloat3("Size", &size.x, 2, ImGuiInputTextFlags_EnterReturnsTrue))
				UpdateCollider();
			break;
		case COLL_SPHERE:
			if (ImGui::InputFloat("Radius", &rad, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
				UpdateCollider();
			break;
		case COLL_PLANE:
			if (ImGui::InputFloat3("Size", &size.x, 2, ImGuiInputTextFlags_EnterReturnsTrue))
				UpdateCollider();
			break;
		case COLL_CAPSULE:
			if ((ImGui::InputFloat("Half height", &size.z, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
				|| (ImGui::InputFloat("Radius", &rad, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue)))
				UpdateCollider();
			break;
		default:
			break;
		}

		// Material
		ImGui::Text("Friction: Static/Dynamic");
		if (ImGui::InputFloat2("", &material.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && physics_body) {
			physics_body->SetMaterial(material.x, material.y, material.z);
		}
		ImGui::Text("Restitution");
		if (ImGui::InputFloat("[0,1]", &material.z, 0.1f, 1.f, 3, ImGuiInputTextFlags_EnterReturnsTrue) && physics_body) {
			physics_body->SetMaterial(material.x, material.y, material.z);
		}
	}
	ImGui::PopID();
}

void RbCollider::SetRigidBodyComp(RigidBody * new_rigid_body)
{
	rigid_body_comp = new_rigid_body;
}

void RbCollider::SetPhysicsBody(jpPhysicsRigidBody * new_physics_body)
{
	if (physics_body && physics_body != new_physics_body && new_physics_body != nullptr)
		delete physics_body;

	physics_body = new_physics_body;
}

jpPhysicsRigidBody * RbCollider::GetPhysicsBody()
{
	return physics_body;
}

void RbCollider::ChangeCollider()
{
	if (physics_body)
		physics_body->SetGeometry(physx::PxVec3(size.x, size.y, size.z), rad, curr_type);
}

void RbCollider::UpdateCollider()
{
	if (physics_body)
		physics_body->SetShapeScale(physx::PxVec3(size.x, size.y, size.z), rad, curr_type);
}

void RbCollider::ChangeParent(GameObject * new_parent)
{
	Component::ChangeParent(new_parent);
	rigid_body_comp = LookForRigidBody();

	if (rigid_body_comp != nullptr)
	{
		rigid_body_comp->SetColliderComp(this);
		physics_body = rigid_body_comp->GetPhysicsBody();
	}
	else
	{
		physics_body = App->physics->GetNewRigidBody(0);
		//Set as Knimeatic(Static) by default
		physics_body->SetDynamic(false);
	}
	// Set Default Gemoetry to Sphere
	physics_body->SetGeometry(physx::PxVec3(size.x, size.y, size.z), rad, curr_type);

	transform = parent->GetTransform();
	UpdateTransform();
}

void RbCollider::Save(const char * buffer_data, char * cursor, int & bytes_copied)
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


	///Collider comp
	bytes_to_copy = sizeof(JP_COLLIDER_TYPE);
	memcpy(cursor, &collider_type, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &curr_type, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//material
	bytes_to_copy = sizeof(float);
	memcpy(cursor, &material.x, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &material.y, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &material.z, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//position
	memcpy(cursor, &position.x, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &position.y, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &position.z, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//size
	memcpy(cursor, &size.x, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &size.y, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &size.z, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//rad
	memcpy(cursor, &rad, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void RbCollider::Load(char * cursor, int & bytes_copied)
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

	///Collider comp
	bytes_to_copy = sizeof(JP_COLLIDER_TYPE);
	memcpy(&collider_type, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&curr_type, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//material
	bytes_to_copy = sizeof(float);
	memcpy(&material.x, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&material.y, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&material.z, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//position
	memcpy(&position.x, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&position.y, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&position.z, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//size
	memcpy(&size.x, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&size.y, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&size.z, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//rad
	memcpy(&rad, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void RbCollider::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(JP_COLLIDER_TYPE) * 2;
	buffer_size += sizeof(float) * 3 * 3; //3 float 3: material, pos and size
	buffer_size += sizeof(float); //rad
}

RigidBody * RbCollider::LookForRigidBody()
{
	if (parent != nullptr)
	{
		for (uint i = 0; i < parent->components.size(); i++)
		{
			if (parent->components[i]->GetType() == COMP_TYPE::COMP_RIGIDBODY)
			{
				return (RigidBody*)parent->components[i];
			}
		}
	}
	return nullptr;
}
