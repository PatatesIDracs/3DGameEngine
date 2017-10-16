#include "Mesh.h"
#include "Glew\include\glew.h"

Mesh::Mesh()
{
}

Mesh::Mesh(GameObject* parent, RenderData* render_data, bool isactive) : Component(parent, COMP_MESH, isactive), render_data(render_data)
{
	bounding_box.SetNegativeInfinity();
	for (uint i = 0; i < render_data->num_vertices * 3; i += 3)
	{
		bounding_box.Enclose(vec(render_data->vertices[i], render_data->vertices[i + 1], render_data->vertices[i + 2]));
	}
}

Mesh::~Mesh()
{
	if (render_data->num_indices > 0)glDeleteBuffers(1, &render_data->id_indices);
	if (render_data->num_vertices > 0)glDeleteBuffers(1, &render_data->id_vertices);
	if (render_data->num_normals > 0)glDeleteBuffers(1, &render_data->id_normals);
	if (render_data->num_tex_vertices > 0)	glDeleteBuffers(1, &render_data->id_tex_vertices);

	delete[] render_data->indices;
	delete[] render_data->vertices;
	delete[] render_data->normals;
	delete[] render_data->tex_vertices;

	delete render_data;
}

const RenderData* Mesh::GetRenderData()
{
	return render_data;
}

void Mesh::Update()
{
	Render();
}

void Mesh::DrawComponent()
{
	if (ImGui::CollapsingHeader("Geometry"))
	{
		ImGui::InputInt("Vertices:", (int*)&render_data->num_vertices, 0, 100, ImGuiInputTextFlags_ReadOnly);

		int faces = render_data->num_indices / 3;
		ImGui::InputInt("Faces:", &faces, 0, 100, ImGuiInputTextFlags_ReadOnly);
	}
}

void Mesh::Render()
{
	//Enable opengl states
	glEnableClientState(GL_VERTEX_ARRAY);
	if (render_data->tex_vertices != nullptr)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Bind texture coords buffer
		glBindBuffer(GL_ARRAY_BUFFER, render_data->id_tex_vertices);
		glTexCoordPointer(3, GL_FLOAT, 0, NULL);
	}

	//Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, render_data->id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Bind and draw with indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->id_indices);
	glDrawElements(GL_TRIANGLES, render_data->num_indices, GL_UNSIGNED_INT, NULL);

	//Disable opengl states
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// Clear Bind buffers
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
