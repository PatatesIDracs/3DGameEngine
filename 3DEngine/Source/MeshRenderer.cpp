#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Glew\include\glew.h"

MeshRenderer::MeshRenderer(GameObject* parent) : Component(parent, COMP_MESHRENDERER)
{
	GetElements();
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Update()
{
	if (transform == nullptr)
		return;

	glPushMatrix();
	glMultMatrixf(transform->GetRotMat().M);

	if (material != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, material->GetTextureID());
	}

	if (mesh != nullptr)
	{
		const RenderData* mesh_render_data = mesh->GetRenderData();

		glEnableClientState(GL_VERTEX_ARRAY);

		if (mesh_render_data->tex_vertices != nullptr)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			//Bind texture coords buffer
			glBindBuffer(GL_ARRAY_BUFFER, mesh_render_data->id_tex_vertices);
			glTexCoordPointer(3, GL_FLOAT, 0, NULL);
		}

		//Bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, mesh_render_data->id_vertices);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		//Bind and draw with indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_render_data->id_indices);
		glDrawElements(GL_TRIANGLES, mesh_render_data->num_indices, GL_UNSIGNED_INT, NULL);

		//Disable opengl states
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	// Clear possible Binded buffers
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();
}

void MeshRenderer::GetElements()
{
	transform = (Transform*)parent->GetTransform();
	mesh = (Mesh*)parent->FindUniqueComponent(COMP_MESH);
	material = (Material*)parent->FindUniqueComponent(COMP_MATERIAL);
}
