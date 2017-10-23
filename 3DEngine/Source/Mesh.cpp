#include "Mesh.h"
#include "Glew\include\glew.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "Transform.h"


Mesh::Mesh()
{
}

Mesh::Mesh(GameObject* parent, RenderData* render_data, bool isactive) : Component(parent, COMP_MESH, isactive), render_data(render_data)
{
	
	aabb_box.SetNegativeInfinity();
	aabb_box.Enclose((float3*)render_data->vertices, render_data->num_vertices);

	obb_box = aabb_box.ToOBB();

	CreateBoxIndices();
	CreateBoxBuffers();

	Transform* rot = (Transform*)parent->FindUniqueComponent(COMP_TRANSFORM);
	if (rot != nullptr) RotateBoundingBox(rot->GetRotQuat());
}

Mesh::~Mesh()
{
	if (render_data->num_indices > 0)glDeleteBuffers(1, &render_data->id_indices);
	if (render_data->num_vertices > 0)glDeleteBuffers(1, &render_data->id_vertices);
	if (render_data->num_normals > 0)glDeleteBuffers(1, &render_data->id_normals);
	if (render_data->num_tex_vertices > 0)	glDeleteBuffers(1, &render_data->id_tex_vertices);
	if (render_data->aabb_vertex_id > 0) glDeleteBuffers(1, &render_data->aabb_vertex_id);
	if (render_data->obb_vertex_id > 0) glDeleteBuffers(1, &render_data->obb_vertex_id);
	if (render_data->box_indices_id > 0) glDeleteBuffers(1, &render_data->box_indices_id);

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
	if (draw_aabb || draw_obb)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glLineWidth(2.0f);

		if (draw_aabb && render_data->aabb_vertex_id > 0)
		{
			//Bind AABB vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, render_data->aabb_vertex_id);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			//Bind and draw with indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->box_indices_id);
			glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);
		}

		if (draw_obb && render_data->obb_vertex_id > 0)
		{
			//Bind OBB vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, render_data->obb_vertex_id);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			//Bind and draw with indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->box_indices_id);
			glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);
		}
		glLineWidth(1.0f);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void Mesh::DrawComponent()
{
	if (ImGui::CollapsingHeader("Geometry"))
	{
		ImGui::Text("the figure"); ImGui::SameLine();
		if (ImGui::Button("Change")) changing_mesh = !changing_mesh;
		if (changing_mesh) ChangeMesh();

		ImGui::InputInt("Vertices:", (int*)&render_data->num_vertices, 0, 100, ImGuiInputTextFlags_ReadOnly);

		int faces = render_data->num_indices / 3;
		ImGui::InputInt("Faces:", &faces, 0, 100, ImGuiInputTextFlags_ReadOnly);

		ImGui::Checkbox("Draw AABB", &draw_aabb);
		ImGui::Checkbox("Draw OBB", &draw_obb);
	}
}

void Mesh::ChangeMesh()
{
	std::string* new_mesh_path = nullptr;
	if (App->editor->DrawLibraryExplorer(new_mesh_path))
	{
		changing_mesh = false;
	}

}

void Mesh::RotateBoundingBox(const math::Quat &transform)
{
	obb_box.Transform(transform);

	aabb_box.SetNegativeInfinity();
	aabb_box.Enclose(obb_box);
	CreateBoxBuffers();
}

// Create box indices buffer (only once)
void Mesh::CreateBoxIndices()
{
	if (render_data->box_indices_id > 0)
	{
		glDeleteBuffers(1, &render_data->box_indices_id);
	}

	Primitive indices;
	render_data->box_indices_id = indices.GenerateBBoxIndices();
}

// Generate AABB and OBB buffers
void Mesh::CreateBoxBuffers()
{
	// delete buffers;
	if (render_data->aabb_vertex_id > 0)
	{
		glDeleteBuffers(1, &render_data->aabb_vertex_id);
	}

	if (render_data->obb_vertex_id > 0)
	{
		glDeleteBuffers(1, &render_data->obb_vertex_id);
	}

	float new_aabb[24]
	{
		aabb_box.CornerPoint(0).x, aabb_box.CornerPoint(0).y,	aabb_box.CornerPoint(0).z,
		aabb_box.CornerPoint(1).x, aabb_box.CornerPoint(1).y,	aabb_box.CornerPoint(1).z,
		aabb_box.CornerPoint(2).x, aabb_box.CornerPoint(2).y,	aabb_box.CornerPoint(2).z,
		aabb_box.CornerPoint(3).x, aabb_box.CornerPoint(3).y,	aabb_box.CornerPoint(3).z,
		aabb_box.CornerPoint(4).x, aabb_box.CornerPoint(4).y,	aabb_box.CornerPoint(4).z,
		aabb_box.CornerPoint(5).x, aabb_box.CornerPoint(5).y,	aabb_box.CornerPoint(5).z,
		aabb_box.CornerPoint(6).x, aabb_box.CornerPoint(6).y,	aabb_box.CornerPoint(6).z,
		aabb_box.CornerPoint(7).x, aabb_box.CornerPoint(7).y,	aabb_box.CornerPoint(7).z,
	};

	float new_obb[24]
	{
		obb_box.CornerPoint(0).x, obb_box.CornerPoint(0).y,	obb_box.CornerPoint(0).z,
		obb_box.CornerPoint(1).x, obb_box.CornerPoint(1).y,	obb_box.CornerPoint(1).z,
		obb_box.CornerPoint(2).x, obb_box.CornerPoint(2).y,	obb_box.CornerPoint(2).z,
		obb_box.CornerPoint(3).x, obb_box.CornerPoint(3).y,	obb_box.CornerPoint(3).z,
		obb_box.CornerPoint(4).x, obb_box.CornerPoint(4).y,	obb_box.CornerPoint(4).z,
		obb_box.CornerPoint(5).x, obb_box.CornerPoint(5).y,	obb_box.CornerPoint(5).z,
		obb_box.CornerPoint(6).x, obb_box.CornerPoint(6).y,	obb_box.CornerPoint(6).z,
		obb_box.CornerPoint(7).x, obb_box.CornerPoint(7).y,	obb_box.CornerPoint(7).z,
	};
	
	Primitive box;
	render_data->aabb_vertex_id = box.GenerateBBoxVertices(new_aabb);
	render_data->obb_vertex_id = box.GenerateBBoxVertices(new_obb);
}