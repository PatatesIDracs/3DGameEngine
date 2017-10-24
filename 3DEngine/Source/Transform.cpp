#include "Transform.h"
#include "Glew\include\glew.h"

#include "Mesh.h"

Transform::Transform(GameObject* parent) : Component(parent, COMP_TRANSFORM, true), transform(float4x4::identity)
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

const Quat Transform::GetRotQuat()
{
	return  Quat::FromEulerXYZ(angle.x*DEGTORAD, angle.y*DEGTORAD, angle.z*DEGTORAD);
}

float3 Transform::GetPosition() const
{
	return position;
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
}

void Transform::UpdateTransform()
{
	// Set Rotation
	Quat rot_q = GetRotQuat();

	// Set Scale
	transform = math::float4x4::Scale(float3(scale.x, scale.y, scale.z), float3(0,0,0))*rot_q.ToFloat4x4();
	
	// Set Position
	transform.Translate(position);

	// Rotate Bounding Box
	rotation.Inverse();
	Mesh* mesh = (Mesh*)parent->FindUniqueComponent(COMP_MESH);
	if (mesh != nullptr)
	{
		mesh->RotateBoundingBox(rotation);
		mesh->RotateBoundingBox(rot_q);
	}
	rotation = rot_q;

	update_transform = false;
}

void Transform::SetTransform(float4x4 &transf)
{
	transform = transf;
	transform.Decompose(position, rotation, scale);
	scale = float3(1.f, 1.f, 1.f);
	angle = rotation.ToEulerXYZ()*RADTODEG;

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
