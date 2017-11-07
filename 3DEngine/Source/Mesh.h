#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"

struct RenderData
{
	~RenderData();

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
	uint box_indices_id = 0;

	//Path to the imported mesh
	char* mesh_path = nullptr;	//TODO: Mesh_path should be part of Mesh not RenderData
};


class Mesh : public Component
{
public:

	Mesh(GameObject* parent, RenderData* render_data, bool isactive = true);
	~Mesh();

	const RenderData* GetRenderData();

	void Update();
	void UpdateTransform();

	void DrawComponent();

	void ChangeMesh();

	void CreateBoxIndices();
	void CreateBoxBuffers(AABB &box);

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(const char* buffer_data, char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

public:

	RenderData* render_data;
	Quat prev_rotation = Quat::identity;

	math::AABB aabb_box;
	bool draw_aabb = true;

	bool changing_mesh = false;
};


#endif // !__MESH_H__