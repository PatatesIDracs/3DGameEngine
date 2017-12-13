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

	physics_body = App->physics->GetNewRigidBody(0);
	//Set as Knimeatic(Static) by default
	physics_body->SetDynamic(false);

	// Set Default Gemoetry to Sphere
	physics_body->SetGeometry(physx::PxVec3(size.x,size.y,size.z), rad, physx::PxGeometryType::Enum::eSPHERE);
}

RbCollider::~RbCollider()
{
}

void RbCollider::Update()
{
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
		ImGui::Combo("Collider", (int*)&collider_type, "Sphere\0Plane\0Box\0Capsule\0", 4);
		if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && physics_body) {
			physx::PxTransform transf =	physics_body->px_body->getGlobalPose();
			transf.p = physx::PxVec3(position.x, position.y, position.z);
			physics_body->px_body->setGlobalPose(transf);
		}
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
			break;
		case COLL_CAPSULE:
			if ((ImGui::InputFloat("Half height", &size.z, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			|| (ImGui::InputFloat("Radius", &rad, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue)))
				UpdateCollider();
			break;
		default:
			break;
		}

		if (curr_type != collider_type && ImGui::Button("Change Collider")) {
			ChangeCollider();
			curr_type = collider_type;
		}
	}
	ImGui::PopID();
}

void RbCollider::Save(const char * buffer_data, char * cursor, int & bytes_copied)
{
}

void RbCollider::Load(char * cursor, int & bytes_copied)
{
}

void RbCollider::GetOwnBufferSize(uint & buffer_size)
{
}

void RbCollider::ChangeParent(GameObject * new_parent)
{
	Component::ChangeParent(new_parent);
	rigid_body_comp = LookForRigidBody();

	transform = parent->GetTransform();
	UpdateTransform();
}

void RbCollider::ChangeCollider()
{
	if (physics_body)
		physics_body->SetGeometry(physx::PxVec3(size.x, size.y, size.z), rad, (physx::PxGeometryType::Enum)curr_type);
}

void RbCollider::UpdateCollider()
{
	if(physics_body)
		physics_body->SetShapeScale(physx::PxVec3(size.x, size.y, size.z), rad);
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
