#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__
 
#include "Color.h"
#include "Math.h"
#include <list>
#include <vector>

enum PRIMITIVE_TYPES
{
	PRIM_POINT,
	PRIM_LINE = 0,
	PRIM_PLANE,
	PRIM_CUBE,
	PRIM_SPHERE,
	PRIM_CYLINDER
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
	void			SetPos(float3 pos);
	void			SetRotation(float angle, const vec &u);
	void			Scale(float x, float y, float z);
	PRIMITIVE_TYPES	GetType() const;
	vec				GetPosition() const;

	uint			GenerateBBoxVertices(float* vertices);
	uint			GenerateBBoxIndices();
public:

	Color color;
	float4x4 transform;
	bool axis, wire;

protected:
	PRIMITIVE_TYPES type;

	Primitive_Data render_data;
	AABB bounding_box;

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
	vec size;
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

	mutable std::list<vec> mesh;
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
	vec origin;
	vec destination;
};

// ============================================
class oldPlane : public Primitive
{
public:
	oldPlane();
	oldPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec normal;
	float constant;
};

#endif // !__PRIMITIVE_H__