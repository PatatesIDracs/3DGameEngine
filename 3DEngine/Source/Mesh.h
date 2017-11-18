#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"

class ResourceMesh;
struct RenderData;

class Mesh : public Component
{
public:

	Mesh(GameObject* parent, ResourceMesh* mesh_resource = nullptr ,bool isactive = true);
	~Mesh();

	const RenderData* GetRenderData() const;

	void Update();
	void UpdateTransform();

	void DrawComponent();

	bool CheckRayCollision(const LineSegment ray, float &dist, float3 &point);

	void ChangeMesh();

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

public:
	ResourceMesh* mesh_resource = nullptr;
	int resource_uid = 0;

	bool draw_aabb = false;

	bool changing_mesh = false;
};


#endif // !__MESH_H__