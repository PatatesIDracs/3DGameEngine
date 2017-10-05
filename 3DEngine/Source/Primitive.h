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

struct Primitive_Data
{
	uint id_indices = 0; // id in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_face_normals = 0;
	uint num_face_normals = 0;
	float*	face_normals_pointer;
};

// -------------------------------
class Primitive
{
public:

	Primitive();

	virtual void	Render(bool face_normals = false) const;
	virtual void	InnerRender() const;
	virtual void	FaceNormalsRender() const;
	virtual void	PrepareToRender();
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

	Primitive_Data render_data;

	void CalculateFaceNormals(float* vertices);
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
	void FaceNormalsRender() const;
public:
	float radius;

private:
	int stacks = 0;
	int slices = 0;

	mutable std::list<vec3> mesh;
	mutable std::vector<float> vertex_array;
	mutable std::vector<float3> face_normals;
	mutable std::vector<float>	normal_array;
	mutable std::vector<float>	face_center;
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