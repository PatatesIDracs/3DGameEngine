#include "Mesh.h"
#include "Glew\include\glew.h"

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
	if (render_data->aabb_vertices != nullptr) glDeleteBuffers(1, &render_data->aabb_vertex_id);
	if (render_data->obb_vertices != nullptr) glDeleteBuffers(1, &render_data->obb_vertex_id);
	if (render_data->box_indices != nullptr) glDeleteBuffers(1, &render_data->box_indices_id);

	delete[] render_data->indices;
	delete[] render_data->vertices;
	delete[] render_data->normals;
	delete[] render_data->tex_vertices;
	delete[] render_data->aabb_vertices;
	delete[] render_data->obb_vertices;
	delete[] render_data->box_indices;

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

		ImGui::Checkbox("Draw AABB", &draw_aabb);
		ImGui::Checkbox("Draw OBB", &draw_obb);
	}
}

void Mesh::RotateBoundingBox(const math::Quat &transform)
{
	OBB box = obb_box;
	box.Transform(transform);

	aabb_box.SetNegativeInfinity();
	aabb_box.Enclose(box);
	CreateBoxBuffers();
}

// Create box indices buffer (only once)
void Mesh::CreateBoxIndices()
{
	if (render_data->box_indices != nullptr)
	{
		glDeleteBuffers(1, &render_data->box_indices_id);
		delete[] render_data->box_indices;
	}

	uint new_indices[24] = {	
		0,1,
		0,2,
		0,4,
		1,3,
		1,5,
		2,3,
		2,6,
		3,7,
		4,5,
		4,6,
		5,7,
		6,7
	};

	render_data->box_indices = new uint[24];
	memcpy(render_data->box_indices, new_indices, sizeof(uint) * 24);

	glGenBuffers(1, (GLuint*)&render_data->box_indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->box_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 24, render_data->box_indices, GL_STATIC_DRAW);
}

// Generate AABB and OBB buffers
void Mesh::CreateBoxBuffers()
{
	// delete buffers;
	if (render_data->aabb_vertices != nullptr)
	{
		glDeleteBuffers(1, &render_data->aabb_vertex_id);
	}
	else render_data->aabb_vertices = new float[24];

	if (render_data->obb_vertices != nullptr)
	{
		glDeleteBuffers(1, &render_data->obb_vertex_id);
	}
	else render_data->obb_vertices = new float[24];

	float new_aabb[]
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

	float new_obb[]
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

	memcpy(render_data->aabb_vertices, new_aabb, sizeof(float)* 24);
	memcpy(render_data->obb_vertices, new_obb, sizeof(float) * 24);

	glGenBuffers(1, (GLuint*)&render_data->aabb_vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, render_data->aabb_vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, render_data->aabb_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&render_data->obb_vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, render_data->obb_vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, render_data->obb_vertices, GL_STATIC_DRAW);

}