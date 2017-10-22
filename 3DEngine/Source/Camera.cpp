#include "Camera.h"
#include "Transform.h"

#include "Application.h"
#include "ModuleWindow.h"

#include "Glew\include\glew.h"

Camera::Camera(GameObject * parent, bool isactive) : Component(parent, COMP_CAMERA, isactive)
{
	cfrustum = new Frustum();
	cfrustum->SetKind(FrustumSpaceGL, FrustumLeftHanded);
	cfrustum->SetViewPlaneDistances(MIN_NEARP_DIST, MIN_FARP_DIST);
	if (parent != nullptr)
	{
		Transform* ptransf = (Transform*)parent->FindUniqueComponent(COMP_TRANSFORM);
		if (ptransf != nullptr)
		{
			mat4x4 transf = ptransf->GetRotMat();
			cfrustum->SetFrame(vec(transf.M[12], transf.M[13], transf.M[14]), vec(transf.M[8],transf.M[9],transf.M[10]), vec(transf.M[4], transf.M[5], transf.M[6]));
		}
	}
	else cfrustum->SetFrame(vec(0.f, 0.f, 0.f), vec(0.f, 0.f, -1.f), vec(0.f, 1.f, 0.f));
	cfrustum->SetPerspective(1024, 720);
}

Camera::~Camera()
{
	delete cfrustum;
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
}

void Camera::DrawComponent()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Checkbox("Active", &active);

		if (ImGui::InputInt("Near Plane", (int*)&near_plane, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue)
			|| ImGui::InputInt("Far Plane", (int*)&far_plane, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			SetFrustumPlanes();

		if (ImGui::SliderFloat("Field of View", &field_of_view, 45.f, 100.f, "%.2f"))
			SetFrustumViewAngle();
	}
}

float* Camera::GetViewMatrix()
{
	float3x4 world_mat = cfrustum->WorldMatrix();
	vec3 X = vec3(world_mat.Col(0));
	vec3 Y = vec3(world_mat.Col(1));
	vec3 Z = vec3(world_mat.Col(2));
	vec3 Pos = vec3(world_mat.Col(3)); 

	return &mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Pos), -dot(Y, Pos), -dot(Z, Pos), 1.0f);
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
