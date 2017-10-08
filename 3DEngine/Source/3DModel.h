#ifndef __3D_MODEL__
#define __3D_MODEL__

#include "Globals.h"
#include "Math.h"
#include "glmath.h"

struct body_mesh
{
	uint id_indices = 0; // id in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	float*	normals = nullptr;

	uint id_tex_vertices = 0;
	uint num_tex_vertices = 0;
	float* tex_vertices = nullptr;

	uint id_texture = 0;
};

class Body3D
{
public:
	Body3D();
	Body3D(body_mesh nmesh, mat4x4 ntransform);
	~Body3D();

	body_mesh GetMesh() const;
	vec3      GetPosition() const;
	float	  GetBodySize() const;

	void Render() const;

private:

	body_mesh mesh; 
	AABB bounding_box;

	mat4x4 transform;
};

#endif // !__3D_MODEL__