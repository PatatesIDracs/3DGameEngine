#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include "glmath.h"
#include "Color.h"
#include "Globals.h"

struct Mesh_data
{
	uint id_indices = 0; // id in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	float* normals = nullptr;
};


class nMesh
{
public:
	nMesh();
	nMesh(Mesh_data data);
	~nMesh();

	int GetIndicesBufferID() const;
	int GetVertexBufferID() const;
	int GetNumberOfIndices() const;
	
private:
	int buffer_id;

	Mesh_data own_mesh;

};

#endif // !__PRIMITIVES_H__
