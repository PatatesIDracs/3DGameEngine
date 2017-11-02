#include "Transform.h"
#include "Glew\include\glew.h"

#include "Mesh.h"

Transform::Transform(GameObject* parent) : Component(parent, COMP_TRANSFORM, true), transform(float4x4::identity), gloval_transform(float4x4::identity)
{
	unique = true;
	update_transform = true;
}

Transform::~Transform()
{
}

const float4x4 Transform::GetRotMat() const
{
	return transform;
}

const float4x4 Transform::GetGlovalTransform() const
{
	return gloval_transform;
}

const Quat Transform::GetRotQuat() const
{
	return  Quat::FromEulerXYZ(angle.x*DEGTORAD, angle.y*DEGTORAD, angle.z*DEGTORAD);
}

float3 Transform::GetPosition() const
{
	return position;
}

float3 Transform::GetScale() const
{
	return scale;
}

void Transform::Update()
{
	if (update_transform)
	{
		for (uint i = 0; i < parent->components.size(); i++)
		{
			parent->components[i]->UpdateTransform();
		}
	}

	if (parent->parent != nullptr && !parent->parent->IsRoot()) {
		gloval_transform = parent->parent->GetTransform()->gloval_transform* transform;
	}
	else gloval_transform = transform;
}

void Transform::UpdateTransform()
{
	// Set Rotation
	rotation = GetRotQuat();

	// Set Transform
	transform = float4x4::FromTRS(position, rotation, scale);

	update_transform = false;
}

void Transform::SetTransform(float4x4 &transf)
{
	transform = transf.Transposed();
	transform.Decompose(position, rotation, scale);
	scale = float3(1.f, 1.f, 1.f);
	angle = rotation.ToEulerXYZ()*RADTODEG;

	UpdateTransform();
	gloval_transform = transform;
	update_transform = true;
}

void Transform::DrawComponent()
{
	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
		if(ImGui::InputFloat3("Rotation", &angle.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
		if(ImGui::InputFloat3("Scale", &scale.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
	}
}
