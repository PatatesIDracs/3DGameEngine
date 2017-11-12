#ifndef __MESH_H__
#define __MESH_H__

#include "Component.h"

class ResourceMesh;
struct RenderData;

class Mesh : public Component
{
public:

	Mesh(GameObject* parent, bool isactive = true);
	~Mesh();

	const RenderData* GetRenderData() const;

	void Update();
	void UpdateTransform();

	void DrawComponent();

	bool CheckRayCollision(const LineSegment ray, float &dist, float3 &point);

	void ChangeMesh();

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(const char* buffer_data, char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

public:
	ResourceMesh* mesh_resource = nullptr;

	bool draw_aabb = false;

	bool changing_mesh = false;
};


#endif // !__MESH_H__