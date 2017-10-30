#include "Camera.h"
#include "Transform.h"

#include "Application.h"
#include "ModuleWindow.h"

#include "Glew\include\glew.h"

Camera::Camera(GameObject * parent, bool isactive) : Component(parent, COMP_CAMERA, isactive)
{
	cfrustum = new Frustum();
	cfrustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
	cfrustum->SetViewPlaneDistances(MIN_NEARP_DIST, 50.f);
	if (parent != nullptr)
	{
		const float* transf = parent->GetTransform()->GetRotMat().ptr();
		cfrustum->SetFrame(vec(transf[12], transf[13], transf[14]), -vec(transf[8],transf[9],transf[10]), vec(transf[4], transf[5], transf[6]));
	}
	else
	{
		LOGC("WARNING: Component Camera Parent is NULL");
		cfrustum->SetFrame(vec(2.f, 2.f, 2.f), vec(0.f, 0.f, -1.f), vec(0.f, 1.f, 0.f));
	}
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

void Camera::UpdateTransform()
{
	Transform* transf = parent->GetTransform();

	float3x3 mat = transf->GetRotMat().Transposed().Float3x3Part();
	cfrustum->SetFrame(transf->GetPosition(), -mat.Col(2), mat.Col(1));

	//Temporal
	GenerateFrostumDraw();
}

void Camera::DrawComponent()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Checkbox("Active", &active);

		if (ImGui::InputFloat("Near Plane", &near_plane, 0, 100,2, ImGuiInputTextFlags_EnterReturnsTrue)
		||ImGui::InputFloat("Far Plane", &far_plane, 0, 100,2, ImGuiInputTextFlags_EnterReturnsTrue))
			SetFrustumPlanes();

		if (ImGui::SliderInt("Field of View", &field_of_view, 45.f, 100.f, "%.2f"))
			SetFrustumViewAngle();

		ImGui::InputFloat("Aspect Ratio", &aspect_ratio, 0, 100, 3, ImGuiInputTextFlags_ReadOnly);
	}
}

float4x4 Camera::GetProjMatrix() const
{
	return cfrustum->ComputeProjectionMatrix();
}

float* Camera::GetViewMatrix()
{
	return cfrustum->ViewProjMatrix().Transposed().ptr();
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
