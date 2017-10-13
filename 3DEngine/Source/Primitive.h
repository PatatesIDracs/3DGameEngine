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
	Primitive_Plane,
};

struct Primitive_Data
{
	uint id_indices = 0; // id in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_face_normals = 0; //id in VRAM
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
	vec3			GetPosition() const;


public:

	Color color;
	mat4x4 transform;
	bool axis, wire;

protected:
	PrimitiveTypes type;

	Primitive_Data render_data;
	AABB bounding_box;

	void CalculateFaceNormals(float* vertices);
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