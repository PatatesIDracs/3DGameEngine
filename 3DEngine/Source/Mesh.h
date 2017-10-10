#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"
#include "Math.h"


struct RenderData
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

};


class Mesh : public Component
{
public:
	Mesh();
	Mesh(RenderData render_data);
	~Mesh();

	const RenderData* GetRenderData();

	void Update();

private:
	void Render();

public:

	RenderData render_data;
	math::AABB bounding_box;


};


#endif // !__MESH_H__