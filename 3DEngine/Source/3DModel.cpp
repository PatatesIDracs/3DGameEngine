#include "3DModel.h"
#include "Glew\include\glew.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>

Body3D::Body3D()
{
}

Body3D::Body3D(body_mesh nmesh, mat4x4 ntransform) : mesh(nmesh), transform(ntransform)
{
	for (uint i = 0; i < mesh.num_vertices * 3; i += 3)
	{
		bounding_box.Enclose(vec(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]));
		if (i == 0) bounding_box.minPoint = bounding_box.maxPoint;
	}
}

Body3D::~Body3D()
{
	if (mesh.num_indices > 0)glDeleteBuffers(1, &mesh.id_indices);
	if (mesh.num_vertices > 0)glDeleteBuffers(1, &mesh.id_vertices);
	if (mesh.num_normals > 0)glDeleteBuffers(1, &mesh.id_normals);
}

body_mesh Body3D::GetMesh() const
{
	return mesh;
}

vec3 Body3D::GetPosition() const
{
	vec center = bounding_box.minPoint + bounding_box.HalfSize();
	return vec3(center.x, center.y, center.z);
}

float Body3D::GetBodySize() const
{
	vec ret = bounding_box.HalfSize();
	return sqrt(ret.x*ret.x + ret.y*ret.y + ret.z*ret.z);
}

void Body3D::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	// Draw Robot With Indices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	// draw a cube
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_indices);
	glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, NULL);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();
}
