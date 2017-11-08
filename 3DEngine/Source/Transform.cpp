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
	if (parent != nullptr && App->scene_intro->GetSelectedGameObject() == parent) {
		OnGuizmo();
	}

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
	if (parent->parent != nullptr) {
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

void Transform::OnGuizmo()
{
	ImGuizmo::Enable(true);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		guizmo_op = ImGuizmo::OPERATION::TRANSLATE;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		guizmo_op = ImGuizmo::OPERATION::ROTATE;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		guizmo_op = ImGuizmo::OPERATION::SCALE;

	float4x4 matrix = transform.Transposed();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(App->camera->GetViewMatrix4x4().ptr(), App->camera->GetProjMatrix().ptr(), guizmo_op, ImGuizmo::LOCAL, matrix.ptr());

	if (ImGuizmo::IsUsing()) {
		matrix.Transpose();
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
	Update();
}

void Transform::DrawComponent()
{
	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
		if (ImGui::DragFloat3("Rotation", &angle.x, 2)) {
			NormalizeRotationAngle();
			update_transform = true;
		}
		if(ImGui::InputFloat3("Scale", &scale.x, 2, ImGuiInputTextFlags_EnterReturnsTrue)) update_transform = true;
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
}

void Transform::Load(const char * buffer_data, char * cursor, int & bytes_copied)
{
	//UUID and parentUUID
	uint bytes_to_copy = sizeof(int);
	memcpy(&UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&parent_UUID, cursor, bytes_to_copy);
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
	//scale = float3(1.f, 1.f, 1.f);
	angle = rotation.ToEulerXYZ()*RADTODEG;
	update_transform = true;

	LOGC("Component transform loaded");
}

void Transform::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int);			//identifier
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(UUID);
	buffer_size += sizeof(parent_UUID);
	buffer_size += sizeof(float) * 16;		//Transform
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

