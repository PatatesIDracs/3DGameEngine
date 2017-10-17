#include "Mesh.h"
#include "Glew\include\glew.h"

Mesh::Mesh()
{
}

Mesh::Mesh(GameObject* parent, RenderData* render_data, bool isactive) : Component(parent, COMP_MESH, isactive), render_data(render_data)
{
	bounding_box.SetNegativeInfinity();
	bounding_box.Enclose((float3*)render_data->vertices, render_data->num_vertices);
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

void Mesh::DrawComponent()
{
	if (ImGui::CollapsingHeader("Geometry"))
	{
		ImGui::InputInt("Vertices:", (int*)&render_data->num_vertices, 0, 100, ImGuiInputTextFlags_ReadOnly);

		int faces = render_data->num_indices / 3;
		ImGui::InputInt("Faces:", &faces, 0, 100, ImGuiInputTextFlags_ReadOnly);
	}
}

void Mesh::RotateBoundingBox(const math::Quat &transform)
{
	OBB box = bounding_box.ToOBB();
	box.Transform(transform);
	bounding_box.Enclose(box);
}

