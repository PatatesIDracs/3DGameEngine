#include "RbCollider.h"

#include "jpPhysicsShape.h"

#include "Glew\include\glew.h"

RbCollider::RbCollider(GameObject * parent, bool isactive) : Component(parent, COMP_RBSHAPE, true)
{
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
