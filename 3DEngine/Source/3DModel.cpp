#include "3DModel.h"
#include "Glew\include\glew.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>

Body3D::Body3D()
{
}

Body3D::Body3D(BodyMesh nmesh, mat4x4 ntransform) : mesh(nmesh), transform(ntransform)
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
	if (mesh.num_tex_vertices > 0)	glDeleteBuffers(1, &mesh.id_tex_vertices);
	if (glIsTexture(mesh.id_texture) == GL_TRUE)	glDeleteTextures(1, &mesh.id_texture);

	delete[] mesh.indices;
	delete[] mesh.vertices;
	delete[] mesh.normals;
	delete[] mesh.tex_vertices;	
}

BodyMesh Body3D::GetMesh() const
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

	// Draw buffer With Indices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, mesh.id_texture);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_tex_vertices);
	glTexCoordPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_indices);
	glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, NULL);

	// deactivate arrays after drawing
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// Clear Bind buffers
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glPopMatrix();
}
