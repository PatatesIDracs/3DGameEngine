#include "Transform.h"
#include "Glew\include\glew.h"
#include "Imgui\ImGuizmo.h"

#include "Mesh.h"

#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"

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

const Quat Transform::CurrRotQuat() const
{
	return rotation;
}

const Quat Transform::GetGlobalQuat() const
{
	return global_rotation;
}

const Quat Transform::GetParentQuat() const
{
	if (parent->parent != nullptr) {
		return parent->parent->GetTransform()->GetGlobalQuat();
	}
	return Quat(0.f, 0.f, 0.f, 1);;
}

float3 Transform::GetPosition() const
{
	return position;
}

float3 Transform::GetGlobalPos() const
{
	return global_pos;
}

float3 Transform::GetParentPos() const
{
	if (parent->parent != nullptr)
	{
		return parent->parent->GetTransform()->GetPosition();
	}
	return float3(0.f,0.f,0.f);
}

float3 Transform::GetScale() const
{
	return scale;
}

float3 Transform::GetParentScale() const
{
	if (parent->parent != nullptr)
		return parent->parent->GetTransform()->GetGlobalScale();
	return float3(1,1,1);
}

float3 Transform::GetGlobalScale() const
{
	return global_scale;
}

void Transform::Update()
{
	if (parent != nullptr && App->clock.state != APP_PLAY && App->scene_intro->GetSelectedGameObject() == parent) {
		OnGuizmo();
	}

	if (update_transform) {
		// Set Rotation Quaternion
		rotation = GetRotQuat().Normalized();

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
	if (parent->parent != nullptr) {
		global_transform = parent->parent->GetTransform()->GetGlobalTransform()* transform;
		global_transform.Decompose(global_pos, global_rotation,global_scale);
		global_rotation.Normalized();
	}
	else {
		global_pos = position;
		global_transform = transform;
		global_rotation = rotation;
	}

	for (uint i = 0; i < parent->children.size(); i++)
		parent->children[i]->GetTransform()->EnableUpdateTransform();
}

void Transform::EnableUpdateTransform()
{
	update_transform = true;
}

void Transform::OnGuizmo()
{
	if (App->camera->update_camera) return;
	else ImGuizmo::Enable(true);

	float4x4 matrix = global_transform.Transposed();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(App->camera->GetViewMatrix4x4().ptr(), App->camera->GetProjMatrix().ptr(), App->scene_intro->guizmo_op, ImGuizmo::LOCAL, matrix.ptr());

	if (ImGuizmo::IsUsing()) {
		matrix.Transpose();
		if (!parent->IsRoot()) {
			float4x4 global = parent->parent->GetTransform()->GetGlobalTransform().Inverted();
			matrix = global*matrix;
		}
		
		matrix.Decompose(position, rotation, scale);
		angle = rotation.ToEulerXYZ()*RADTODEG;

		update_transform = true;
	}	
}

void Transform::SetTransform(float4x4 &transf)
{
	transform = transf.Transposed();
	transform.Decompose(position, rotation, scale);
	angle = rotation.ToEulerXYZ()*RADTODEG;

	update_transform = true;
}

void Transform::SetTransform(float3 pos, Quat rot)
{
	position = pos;
	angle = rot.ToEulerXYZ()*RADTODEG;

	update_transform = true;
}

void Transform::SetPosition(float3 new_pos)
{
	position = new_pos;

	update_transform = true;
}

void Transform::SetScale(float3 new_scale)
{
	scale = new_scale;

	update_transform = true;
}

void Transform::DrawComponent()
{
	ImGui::PushID(UUID);
	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
		if (ImGui::DragFloat3("Rotation", &angle.x, 2)) {
			NormalizeRotationAngle();
			update_transform = true;
		}
		if(ImGui::InputFloat3("Scale", &scale.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
	}
	ImGui::PopID();
}

void Transform::NormalizeRotationAngle()
{
	float max_angle = 360.0f;
	int num_spins = 0;
	for (uint i = 0; i < 3; i++) {
		num_spins = (int)(angle[i] / max_angle);
		angle[i] = angle[i] - max_angle* num_spins;

		if (angle[i] <= -180.0f) angle[i] += 360.0f;
		else if (angle[i] > 180.0f) angle[i] -= 360.0f;

	}
}
void Transform::Save(const char * buffer_data, char * cursor, int& bytes_copied)
{
	LOGC("Saving transform comp");

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

	//Transform
	bytes_to_copy = sizeof(float) * 16;
	//turn float4x4 class to a float array for easy memcpy
	float t[] = {	transform[0][0] , transform[0][1] , transform[0][2] , transform[0][3] ,
					transform[1][0] , transform[1][1] , transform[1][2] , transform[1][3] ,
					transform[2][0] , transform[2][1] , transform[2][2] , transform[2][3] ,
					transform[3][0] , transform[3][1] , transform[3][2] , transform[3][3] };
	memcpy(cursor, t, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Global scale
	bytes_to_copy = sizeof(float);
	memcpy(cursor, &global_scale.x, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &global_scale.y, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &global_scale.z, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Global pos
	memcpy(cursor, &global_pos.x, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &global_pos.y, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &global_pos.z, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Global rotation
	memcpy(cursor, &global_rotation.x, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &global_rotation.y, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &global_rotation.z, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &global_rotation.w, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

}

void Transform::Load(char * cursor, int & bytes_copied)
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

	float new_transform[16];
	bytes_to_copy = sizeof(float) * 16;
	memcpy(new_transform, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	transform[0][0] = new_transform[0];
	transform[0][1] = new_transform[1];
	transform[0][2] = new_transform[2];
	transform[0][3] = new_transform[3];
	transform[1][0] = new_transform[4];
	transform[1][1] = new_transform[5];
	transform[1][2] = new_transform[6];
	transform[1][3] = new_transform[7];
	transform[2][0] = new_transform[8];
	transform[2][1] = new_transform[9];
	transform[2][2] = new_transform[10];
	transform[2][3] = new_transform[11];
	transform[3][0] = new_transform[12];
	transform[3][1] = new_transform[13];
	transform[3][2] = new_transform[14];
	transform[3][3] = new_transform[15];


	transform.Decompose(position, rotation, scale);
	angle = rotation.ToEulerXYZ()*RADTODEG;
	update_transform = true;


	//Global scale
	bytes_to_copy = sizeof(float);
	memcpy(&global_scale.x, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&global_scale.y, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&global_scale.z, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Global pos
	memcpy(&global_pos.x, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&global_pos.y, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&global_pos.z, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Global rotation
	memcpy(&global_rotation.x, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&global_rotation.y, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&global_rotation.z, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&global_rotation.w, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	LOGC("Component transform loaded");
}

void Transform::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);

	buffer_size += sizeof(float) * 16;		//Transform
	buffer_size += sizeof(float) * 3 * 2;	//Global scale and pos
	buffer_size += sizeof(float) * 4;		//Global rotation

}


