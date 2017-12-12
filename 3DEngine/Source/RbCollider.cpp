#include "RbCollider.h"
#include "Application.h"
#include "ModulePhysics.h"

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
}

void RbCollider::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("Rb Collider"))
	{

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
