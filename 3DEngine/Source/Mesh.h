#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"

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

	// Draw AABB and OBB
	uint aabb_vertex_id = 0;
	float* aabb_vertices = nullptr;

	uint obb_vertex_id = 0;
	float* obb_vertices = nullptr;
	
	uint num_box_indices = 24;
	uint box_indices_id = 0;
	uint* box_indices = nullptr;
};


class Mesh : public Component
{
public:
	Mesh();
	Mesh(GameObject* parent, RenderData* render_data, bool isactive = true);
	~Mesh();

	const RenderData* GetRenderData();

	void Update();

	void DrawComponent();

	// Modify Component
	void RotateBoundingBox(const math::Quat &transform);

	void CreateBoxIndices();
	void CreateBoxBuffers();

public:

	RenderData* render_data;

	math::AABB aabb_box;
	math::OBB obb_box;

	bool draw_aabb = true;
	bool draw_obb = true;
	
};


#endif // !__MESH_H__