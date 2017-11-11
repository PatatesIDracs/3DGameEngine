#ifndef __RESOURCEMESH_H__
#define __RESOURCEMESH_H__

#include "Resource.h"

class ResourceMesh : public Resource
{
public:
	ResourceMesh(int uid);
	~ResourceMesh();


public:
	//Previous RenderData struct
	uint id_indices = 0; // id in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_normals = 0; // id in VRAM
	uint num_normals = 0;
	float*	normals = nullptr;

	uint id_tex_vertices = 0; // id in VRAM
	uint num_tex_vertices = 0;
	float* tex_vertices = nullptr;

	// Draw AABB and OBB
	uint aabb_vertex_id = 0;
	uint box_indices_id = 0;

};

#endif // !__RESOURCESCENE_H__
