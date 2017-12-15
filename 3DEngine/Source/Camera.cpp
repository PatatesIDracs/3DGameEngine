#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"

Camera::Camera(GameObject * parent, bool isactive) : Component(parent, COMP_CAMERA, isactive)
{
	cfrustum = new Frustum();
	cfrustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
	cfrustum->SetViewPlaneDistances(MIN_NEARP_DIST, 50.f);
	if (parent != nullptr)
	{
		Transform* transf = parent->GetTransform();

		float4x4 mat = transf->GetLocalTransform();
		cfrustum->SetFrame(transf->GetPosition(), mat.Col3(2), mat.Col3(1));
	}
	else
	{
		LOGC("WARNING: Component Camera Parent is NULL");
		cfrustum->SetFrame(vec(1.f, 1.f, 1.f), vec(0.f, 0.f, 1.f), vec(0.f, 1.f, 0.f));
	}
	frustum_planes = new Plane[6];
	cfrustum->GetPlanes(frustum_planes);

	if (main_camera)
		App->camera->SetMainCamera(this, true);
}

Camera::~Camera()
{
	if (main_camera)
		App->camera->SetMainCamera(this, false);

	delete cfrustum;
	delete[] frustum_planes;
}

void Camera::Update()
{
	if (active)
	{
		cfrustum->Draw(3.0, float4(1.f, 1.f, 1.f, 1.f));
	}
}

void Camera::UpdateTransform()
{
	if (!own_transf) {
		Transform* transf = parent->GetTransform();

		float4x4 mat = transf->GetLocalTransform();
		cfrustum->SetFrame(transf->GetPosition(), mat.Col3(2), mat.Col3(1));
	}
	else own_transf = false;
}

void Camera::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Checkbox("Main Camera", &main_camera)) {
			App->camera->SetMainCamera(this, main_camera);
		}
		if (ImGui::InputFloat("Near Plane", &near_plane, 0, 100, 2, ImGuiInputTextFlags_EnterReturnsTrue)
			|| ImGui::InputFloat("Far Plane", &far_plane, 0, 100, 2, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			SetFrustumPlanes();
		}
		if (ImGui::SliderInt("Field of View", &field_of_view, 45, 100, "%.2f")) {
			SetFrustumViewAngle();
		}
		ImGui::InputFloat("Aspect Ratio", &aspect_ratio, 0, 100, 3, ImGuiInputTextFlags_ReadOnly);
	}
	ImGui::PopID();
}

float4x4 Camera::GetProjMatrix() const
{
	return cfrustum->ProjectionMatrix().Transposed();
}

float4x4 Camera::GetViewMatrix4x4() const
{
	float4x4 mat = cfrustum->ViewMatrix();
	return mat.Transposed();
}

float* Camera::GetViewMatrix() const
{
	return cfrustum->ViewProjMatrix().Transposed().ptr();
}

void Camera::GetCameraVectors(float3 & position, float3 & front, float3 & up) const
{
	position = cfrustum->Pos();
	front = cfrustum->Front();
	up = cfrustum->Up();
}

Frustum& Camera::GetFrustum() const
{
	return *cfrustum;
}

bool Camera::GetFrustumGameObjecs(std::vector<GameObject*>& dynamic_array, std::vector<MeshRenderer*>& render_this) const
{
	cfrustum->GetPlanes(frustum_planes);

	int contains_gobj_result;
	for (uint curr_obj = 0; curr_obj < dynamic_array.size(); curr_obj++) {
		contains_gobj_result = CONT_OUT;
		if (dynamic_array[curr_obj]->IsActive()){
			contains_gobj_result = ContainsAABB(dynamic_array[curr_obj]->GetBoundaryBox());

			if (contains_gobj_result == CONT_IN || contains_gobj_result == CONT_INTERSECTS) {
				MeshRenderer* mesh = (MeshRenderer*)dynamic_array[curr_obj]->FindFirstComponent(COMP_MESHRENDERER);

				if (mesh != nullptr && mesh->IsActive()) {
					mesh->ready_to_draw = false;
					render_this.push_back(mesh);
				}
			}
		}
	}
	return true;
}

int Camera::ContainsAABB(const AABB &box) const
{
	if (!box.IsFinite()) return CONT_OUT;

	vec vCorner[8];
	box.GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
								  // test all 8 corners against the 6 sides
								  // if all points are behind 1 specific plane, we are out
								  // if we are in with all points, then we are fully in

	int iTotalIn = 0;
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if(frustum_planes[p].normal.Dot(vCorner[i]) >= frustum_planes[p].d){
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if (iInCount == 0)
			return CONT_OUT;
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
		return CONT_IN;
	// we must be partly in then otherwise
	return CONT_INTERSECTS;
}

void Camera::SetFrustumPlanes()
{
	if (near_plane < MIN_NEARP_DIST) near_plane = MIN_NEARP_DIST;
	if (far_plane < MIN_FARP_DIST) far_plane = MIN_FARP_DIST;

	cfrustum->SetViewPlaneDistances(near_plane, far_plane);
}

void Camera::SetFrustumPlanes(float nearp, float farp)
{
	if (nearp < MIN_NEARP_DIST) nearp = MIN_NEARP_DIST;
	if (farp < MIN_FARP_DIST) farp = MIN_FARP_DIST;

	cfrustum->SetViewPlaneDistances(nearp, farp);
}

void Camera::SetFrustumViewAngle()
{
	SetFOVRatio(App->window->width, App->window->height);
}

void Camera::SetFOVRatio(uint width, uint height)
{
	aspect_ratio = (float)width / height;
	float vfov = field_of_view * DEGTORAD;
	float hfov = Atan(aspect_ratio * Tan(vfov * 0.5f)) * 2.0f;

	cfrustum->SetPerspective(hfov, vfov);
}

void Camera::SetNewFrame(vec& pos, vec& front, vec& up)
{
	cfrustum->SetFrame(pos, front, up);

	if (parent != nullptr) {
		float3 angle = cfrustum->WorldMatrix().ToEulerXYZ();
		parent->GetTransform()->SetTransform(pos, Quat::FromEulerXYZ(angle.x, angle.y, angle.z));
		own_transf = true;
	}
}

void Camera::ChangeParent(GameObject * new_parent)
{
	parent = new_parent; 
	parent_UUID = new_parent->GetUUID();
	UpdateTransform();
}

void Camera::Save(const char * buffer_data, char * cursor, int& bytes_copied)
{
	LOGC("Saving camera comp");

	//identifier and type
	int identifier = COMPONENTIDENTIFIER;
	uint bytes_to_copy = sizeof(int);
	memcpy(cursor, &identifier, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(type);
	memcpy(cursor, &type, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//UUID and parent UUID
	bytes_to_copy = sizeof(int);
	memcpy(cursor, &UUID, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
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

	//Add Main Camera Save
	bytes_to_copy = sizeof(bool);
	memcpy(cursor, &main_camera, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//near_plane, far_plane, aspect_ratio, field_of_view
	bytes_to_copy = sizeof(float);
	memcpy(cursor, &near_plane, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &far_plane, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &aspect_ratio, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(int);
	memcpy(cursor, &field_of_view, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void Camera::Load(char * cursor, int & bytes_copied)
{
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

	//Load Main Camera
	bytes_to_copy = sizeof(bool);
	memcpy(&main_camera, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//near_plane, far_plane, aspect_ratio
	bytes_to_copy = sizeof(float);
	memcpy(&near_plane, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&far_plane, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&aspect_ratio, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	//Field of view
	bytes_to_copy = sizeof(int);
	memcpy(&field_of_view, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	LOGC("Component camera loaded");

	SetFrustumPlanes();
	SetFrustumViewAngle();

	if (main_camera)
		App->camera->SetMainCamera(this, true);
}

void Camera::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);

	buffer_size += sizeof(bool);		//is main camera
	buffer_size += sizeof(float) * 3;	//near_plane, far_plane, aspect_ratio
	buffer_size += sizeof(int);			//Field of view	
}
