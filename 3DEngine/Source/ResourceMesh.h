#ifndef __RESOURCEMESH_H__
#define __RESOURCEMESH_H__

#include "Resource.h"
#include "Math.h"

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
};


class ResourceMesh : public Resource
{
public:
	ResourceMesh(int uid);
	~ResourceMesh();

	void SetRenderData(RenderData* new_render_data);
	void LoadToMemory();

	const RenderData*	GetRenderData() const;
	AABB				GetAABB() const;

	void SaveResource(std::string* library_path, std::string* assets_path);
private:

	RenderData* render_data = nullptr;
	math::AABB aabb_box;
};

#endif // !__RESOURCESCENE_H__
