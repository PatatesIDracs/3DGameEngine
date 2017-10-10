#include "Transform.h"
//#include "glmath.h"

Transform::Transform() : Component(nullptr, COMP_TRANSFORM, true)
{
	transform = mat4x4();
}

Transform::Transform(GameObject* parent, mat4x4 transf, vec3 pos, vec3 eu_angles, vec3 scale, bool isactive) : Component(parent, COMP_TRANSFORM, isactive),
transform(transf), position(pos), angle(eu_angles), scale(scale)
{
	
}

Transform::~Transform()
{
}

void Transform::SetAngleFromQuat(Quat& q)
{
	// x axis rotation
	double sinr = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	angle.x = RadToDeg(atan2(sinr, cosr));

	// y axis rotation
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		angle.y = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		angle.y = RadToDeg(asin(sinp));

	// z axis rotation
	double siny = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	angle.z = RadToDeg(atan2(siny, cosy));
}