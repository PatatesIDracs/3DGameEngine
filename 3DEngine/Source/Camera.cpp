#include "Camera.h"
#include "Transform.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"

#include "Glew\include\glew.h"

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
}

Camera::~Camera()
{
	delete cfrustum;
	delete[] frustum_planes;

	if (fvertices_id != 0) glDeleteBuffers(1, &fvertices_id);
	if (findices_id != 0) glDeleteBuffers(1, &findices_id);
}

void Camera::Update()
{
	if (fvertices_id != 0 && findices_id != 0)
	{
		glLineWidth(2.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, fvertices_id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, findices_id);
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);

		glLineWidth(1.0f);
	}
	UpdateTransform();
}

void Camera::UpdateTransform()
{
	Transform* transf = parent->GetTransform();

	float4x4 mat = transf->GetLocalTransform();
	cfrustum->SetFrame(transf->GetPosition(), mat.Col3(2), mat.Col3(1));

	//Temporal
	GenerateFrostumDraw();
}

void Camera::DrawComponent()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Checkbox("Main Camera", &active)) {
			App->camera->SetMainCamera(this, active);
		}
		if (ImGui::InputFloat("Near Plane", &near_plane, 0, 100, 2, ImGuiInputTextFlags_EnterReturnsTrue)
			|| ImGui::InputFloat("Far Plane", &far_plane, 0, 100, 2, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			SetFrustumPlanes();
			GenerateFrostumDraw();
		}
		if (ImGui::SliderInt("Field of View", &field_of_view, 45, 100, "%.2f")) {
			SetFrustumViewAngle();
			GenerateFrostumDraw();
		}
		ImGui::InputFloat("Aspect Ratio", &aspect_ratio, 0, 100, 3, ImGuiInputTextFlags_ReadOnly);
	}
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

Frustum& Camera::GetFrustum() const
{
	return *cfrustum;
}

// Should not be used
bool Camera::GetFrustumGameObjecs(GameObject* root, std::vector<MeshRenderer*>& render_this) const
{
	if (!active) return false;

	cfrustum->GetPlanes(frustum_planes);

	std::vector<GameObject*> check_list;
	check_list.push_back(root);

	int contains_gobj_result = 0;

	uint curr_item = 0;
	GameObject* curr_game_obj = nullptr;
	while (curr_item < check_list.size())
	{
		contains_gobj_result = CONT_OUT;
		curr_game_obj = check_list[curr_item];

		// check if gameobject is inside frustum, only if gameobject is active
		if (curr_game_obj->IsActive())
			contains_gobj_result = ContainsAABB(curr_game_obj->boundary_box);
		else {
			curr_item++;
			continue;
		}

		// Add GameObject to render_this
		if (contains_gobj_result == CONT_IN || contains_gobj_result == CONT_INTERSECTS) {
			MeshRenderer* mesh = (MeshRenderer*)curr_game_obj->FindFirstComponent(COMP_MESHRENDERER);

			if(mesh != nullptr) render_this.push_back(mesh);
		}

		// Add Childs to Check List
		for (uint i = 0; i < curr_game_obj->children.size(); i++)
			check_list.push_back(curr_game_obj->children[i]);

		curr_item++;
	}
	check_list.clear();

	return true;
}

// Use this function instead -----------------
bool Camera::GetFrustumGameObjecs(std::vector<GameObject*>& dynamic_array, std::vector<MeshRenderer*>& render_this) const
{
	if (!active) return false;

	cfrustum->GetPlanes(frustum_planes);

	int contains_gobj_result;
	for (uint curr_obj = 0; curr_obj < dynamic_array.size(); curr_obj++) {
		contains_gobj_result = CONT_OUT;
		if (dynamic_array[curr_obj]->IsActive()){
			contains_gobj_result = ContainsAABB(dynamic_array[curr_obj]->boundary_box);

			if (contains_gobj_result == CONT_IN || contains_gobj_result == CONT_INTERSECTS) {
				MeshRenderer* mesh = (MeshRenderer*)dynamic_array[curr_obj]->FindFirstComponent(COMP_MESHRENDERER);

				if (mesh != nullptr) render_this.push_back(mesh);
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
			if (frustum_planes[p].IsOnPositiveSide(vCorner[i])) {
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

void Camera::GenerateFrostumDraw()
{
	if (fvertices_id != 0) glDeleteBuffers(1, &fvertices_id);

	float n_vertices[] =
	{
		cfrustum->CornerPoint(0).x,cfrustum->CornerPoint(0).y,cfrustum->CornerPoint(0).z,
		cfrustum->CornerPoint(1).x,cfrustum->CornerPoint(1).y,cfrustum->CornerPoint(1).z,
		cfrustum->CornerPoint(2).x,cfrustum->CornerPoint(2).y,cfrustum->CornerPoint(2).z,
		cfrustum->CornerPoint(3).x,cfrustum->CornerPoint(3).y,cfrustum->CornerPoint(3).z,
		cfrustum->CornerPoint(4).x,cfrustum->CornerPoint(4).y,cfrustum->CornerPoint(4).z,
		cfrustum->CornerPoint(5).x,cfrustum->CornerPoint(5).y,cfrustum->CornerPoint(5).z,
		cfrustum->CornerPoint(6).x,cfrustum->CornerPoint(6).y,cfrustum->CornerPoint(6).z,
		cfrustum->CornerPoint(7).x,cfrustum->CornerPoint(7).y,cfrustum->CornerPoint(7).z
	};
	Primitive x;
	fvertices_id = x.GenerateBBoxVertices(n_vertices);

	if (findices_id == 0)
	{
		findices_id = x.GenerateBBoxIndices();
	}
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
}

void Camera::ChangeParent(GameObject * new_parent)
{
	parent = new_parent; 
	parent_UUID = new_parent->UUID;
	UpdateTransform();
}

void Camera::Save(const char * buffer_data, char * cursor, int& bytes_copied)
{
	LOGC("Saving camera comp");

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

void Camera::Load(const char * buffer_data, char * cursor, int & bytes_copied)
{
	uint bytes_to_copy = sizeof(int);
	memcpy(&UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&parent_UUID, cursor, bytes_to_copy);
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
}

void Camera::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int);			//identifier
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(UUID);
	buffer_size += sizeof(parent_UUID);
	buffer_size += sizeof(float) * 3;	//near_plane, far_plane, aspect_ratio
	buffer_size += sizeof(int);			//Field of view	
}
