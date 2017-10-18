#include "Transform.h"
#include "Glew\include\glew.h"

Transform::Transform() : Component(nullptr, COMP_TRANSFORM, true)
{
	transform = mat4x4();
	unique = true;
}

Transform::Transform(Transform & transf) : Component(transf.parent, COMP_TRANSFORM, transf.active), transform(transf.GetRotMat()), position(transf.position),
scale(transf.scale), angle(transf.angle)
{
	unique = true;
}

Transform::Transform(GameObject* parent, mat4x4 transf, bool isactive) : Component(parent, COMP_TRANSFORM, isactive), transform(transf)
{
	position = vec3(transf.M[12], transf.M[13], transf.M[14]);
	scale = vec3(Abs((transf.M[0]+transf.M[1]+transf.M[2])), Abs((transf.M[4] + transf.M[5] + transf.M[6])), Abs((transf.M[8] + transf.M[9] + transf.M[10])));
	transform.M[0] *= 1 / scale.x;
	transform.M[1] *= 1 / scale.x;
	transform.M[2] *= 1 / scale.x;
	transform.M[4] *= 1 / scale.y;
	transform.M[5] *= 1 / scale.y;
	transform.M[6] *= 1 / scale.y;
	transform.M[8] *= 1 / scale.z;
	transform.M[9] *= 1 / scale.z;
	transform.M[10] *= 1 / scale.z;


	GetEAnglesFromMat();
	unique = true;
}

Transform::~Transform()
{
}

const mat4x4 Transform::GetRotMat() const
{
	return transform;
}

// Change Name
void Transform::GetEAnglesFromMat()
{
	// Euler Angles
	if (transform.M[2] != 1 && transform.M[2] != -1)
	{
		angle.y = -asin(transform.M[2]);
		angle.z = atan2(transform.M[6] / math::Cos(angle.y), transform.M[10] / math::Cos(angle.y));
		angle.x = atan2(transform.M[1] / math::Cos(angle.y), transform.M[0] / math::Cos(angle.y));
		angle.y = RadToDeg(angle.y);
	}
	else
	{
		angle.z = 0;
		if (transform.M[2] == -1)
		{
			angle.y = 90;
			angle.x = atan2(transform.M[4], transform.M[8]);
		}
		else
		{
			angle.y = -90;
			angle.x = atan2(-transform.M[4], -transform.M[8]);
		}
	}
	angle.z = RadToDeg(angle.z);
	angle.x = RadToDeg(angle.x);
}

void Transform::DrawComponent()
{
	if (ImGui::CollapsingHeader("Transformation"))
	{
		ImGui::InputFloat3("Position", &position.x, -1, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Rotation", &angle.x, -1, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Scale", &scale.x, -1, ImGuiInputTextFlags_ReadOnly);
	}
}
