#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"

class ResourceMesh;

class Mesh : public Component
{
public:

	Mesh(GameObject* parent, bool isactive = true);
	~Mesh();

//	const RenderData* GetRenderData();

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
	ResourceMesh* mesh_resource = nullptr;

	math::AABB aabb_box;
	bool draw_aabb = true;

	bool changing_mesh = false;
};


#endif // !__MESH_H__