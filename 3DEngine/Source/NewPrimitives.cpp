#include "Glew\include\glew.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Math.h"
#include "NewPrimitives.h"


nMesh::nMesh()
{

}

nMesh::nMesh(Mesh_data data)
{
	own_mesh = data;

	glGenBuffers(1, (GLuint*)&own_mesh.id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, own_mesh.id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(own_mesh.vertices), &own_mesh.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&own_mesh.id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, own_mesh.id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, own_mesh.num_indices * sizeof(uint), &own_mesh.indices[0], GL_STATIC_DRAW);

}

nMesh::~nMesh()
{
	glDeleteBuffers(1, &own_mesh.id_vertices);
	glDeleteBuffers(1, &own_mesh.id_indices);
}

int nMesh::GetIndicesBufferID() const
{
	return own_mesh.id_indices;
}

int nMesh::GetVertexBufferID() const
{
	return own_mesh.id_vertices;
}

int nMesh::GetNumberOfIndices() const
{
	return own_mesh.num_indices;
}

