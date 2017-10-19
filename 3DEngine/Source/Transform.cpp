#include "Transform.h"
#include "Glew\include\glew.h"

#include "Mesh.h"

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
	scale = vec3(1, 1, 1);
	SetScale();
	
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

const Quat Transform::GetRotQuat()
{
	return  Quat::FromEulerXYZ(angle.x*DEGTORAD, angle.y*DEGTORAD, angle.z*DEGTORAD);
}

void Transform::SetPosition()
{
	transform.translate(position.x, position.y,  position.z);
}

void Transform::SetRotation()
{
	// Temporal Function need to improve
	Quat rot_q = Quat::FromEulerXYZ(angle.x*DEGTORAD, angle.y*DEGTORAD, angle.z*DEGTORAD);

	float rot_a = rot_q.Angle();
	vec axis;
	if (rot_a == 0) axis = vec(1, 0, 0);
	else axis = rot_q.Axis();
	transform = transform.rotate(rot_q.Angle()*RADTODEG,vec3(axis.x, axis.y, axis.z));

	Mesh* mesh = (Mesh*)parent->FindUniqueComponent(COMP_MESH);
	if (mesh != nullptr)
	{
		mesh->RotateBoundingBox(rot_q);
	}
}

void Transform::SetScale()
{
	// Temporal Function need to improve
	vec3 curr_scale = vec3(Abs((transform.M[0] + transform.M[1] + transform.M[2])), Abs((transform.M[4] + transform.M[5] + transform.M[6])), Abs((transform.M[8] + transform.M[9] + transform.M[10])));
	float x = scale.x / curr_scale.x;
	float y = scale.y / curr_scale.y;
	float z = scale.z / curr_scale.z;
	if (x != 0)
	{
		transform.M[0] *= x;
		transform.M[1] *= x;
		transform.M[2] *= x;
	}
	if (y != 0)
	{
		transform.M[4] *= y;
		transform.M[5] *= y;
		transform.M[6] *= y;
	}
	if (z != 0)
	{
		transform.M[8] *= z;
		transform.M[9] *= z;
		transform.M[10] *= z;
	}
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
		if(ImGui::InputFloat3("Position", &position.x, 2)) SetPosition();
		if(ImGui::InputFloat3("Rotation", &angle.x, 2)) SetRotation();
		if(ImGui::InputFloat3("Scale", &scale.x, 2)) SetScale();
	}
}
