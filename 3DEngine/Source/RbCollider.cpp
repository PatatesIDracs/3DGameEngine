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
	//TODO: do it diferently
	physics_body->SetSphereGeometry(1);
	physics_body->ActivateShape();
}

RbCollider::~RbCollider()
{
}

void RbCollider::Update()
{
}

void RbCollider::UpdateTransform()
{
	if ((rigid_body_comp && !rigid_body_comp->own_update) || physics_body) {
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
		ImGui::Combo("Collider", (int*)&collider_type, "Box\0Sphere\0Plane\0", 3);
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
			if (ImGui::InputFloat("Radius", &rad, 0.1, 1, 2, ImGuiInputTextFlags_EnterReturnsTrue))
				UpdateCollider();
			break;
		case COLL_PLANE:
			break;
		default:
			break;
		}

		if (curr_type != collider_type) {
			if (ImGui::Button("Change Collider"));
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
	LookForRigidBody();

	transform = parent->GetTransform();
	UpdateTransform();
}

void RbCollider::ChangeCollider()
{
	//TODO: Create Collider of type curr_type
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
