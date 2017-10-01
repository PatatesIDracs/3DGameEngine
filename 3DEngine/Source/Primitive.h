#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "glmath.h"
#include "Color.h"
#include "Math.h"
#include <list>
#include <vector>

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:

	Color color;
	mat4x4 transform;
	bool axis, wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class oldCube : public Primitive
{
public:
	oldCube();
	oldCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class oldSphere : public Primitive
{
public:
	oldSphere();
	oldSphere(float radius);
	~oldSphere();
	void InnerRender() const;
public:
	float radius;

private:
	math::Sphere geo_sphere;

	mutable float3*	vertices3 = nullptr;
	mutable float2* vertices3_uv = nullptr;

	int stacks = 0;
	int slices = 0;
	
	mutable std::list<vec3> mesh;
	mutable std::vector<float> vertex_array;
};

// ============================================
class oldCylinder : public Primitive
{
public:
	oldCylinder();
	oldCylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class oldLine : public Primitive
{
public:
	oldLine();
	oldLine(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class oldPlane : public Primitive
{
public:
	oldPlane();
	oldPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};

#endif // !__PRIMITIVE_H__