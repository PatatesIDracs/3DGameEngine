#include "Transform.h"
#include "Glew\include\glew.h"

#include "Mesh.h"

Transform::Transform(GameObject* parent) : Component(parent, COMP_TRANSFORM, true), transform(float4x4::identity), global_transform(float4x4::identity)
{
	unique = true;
	update_transform = true;
}

Transform::~Transform()
{
}

const float4x4 Transform::GetLocalTransform() const
{
	return transform;
}

const float4x4 Transform::GetGlobalTransform() const
{
	return global_transform;
}

const float * Transform::GetGlobalTransposed() const
{
	return global_transform.Transposed().ptr();
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
	if (update_transform) {
		// Set Rotation
		rotation = GetRotQuat();

		// Set Transform
		transform = float4x4::FromTRS(position, rotation, scale);

		UpdateGlobalTransform();

		for (uint i = 1; i < parent->components.size(); i++)
			parent->components[i]->UpdateTransform();

		update_transform = false;
	}
}

void Transform::UpdateGlobalTransform()
{
	if (parent->parent != nullptr && !parent->parent->IsRoot()) {
		global_transform = parent->parent->GetTransform()->GetGlobalTransform()* transform;
	}
	else global_transform = transform;

	for (uint i = 0; i < parent->children.size(); i++)
		parent->children[i]->GetTransform()->EnableUpdateTransform();
}

void Transform::EnableUpdateTransform()
{
	update_transform = true;
}

void Transform::SetTransform(float4x4 &transf)
{
	transform = transf.Transposed();
	transform.Decompose(position, rotation, scale);
	scale = float3(1.f, 1.f, 1.f);
	angle = rotation.ToEulerXYZ()*RADTODEG;

	update_transform = true;
	Update();
}

void Transform::DrawComponent()
{
	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
		if (ImGui::DragFloat3("Rotation", &angle.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) {
			NormalizeRotationAngle();
			update_transform = true;
		}
		if(ImGui::InputFloat3("Scale", &scale.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
	}
}

void Transform::NormalizeRotationAngle()
{
	float max_angle = 360.0f;
	uint num_spins = 0;
	for (uint i = 0; i < 3; i++) {
		num_spins = (int)angle[i] / max_angle;
		angle[i] = angle[i] - max_angle* num_spins;
		if (angle[i] > 180.0f) angle[i] -= 360.0f;
	}
}
