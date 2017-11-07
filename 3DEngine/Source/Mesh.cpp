#include "Mesh.h"
#include "Glew\include\glew.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "Transform.h"

Mesh::Mesh(GameObject* parent, RenderData* render_data, bool isactive) : Component(parent, COMP_MESH, isactive), render_data(render_data)
{
	
	aabb_box.SetNegativeInfinity();
	if (render_data != nullptr)
	{
		aabb_box.Enclose((float3*)render_data->vertices, render_data->num_vertices);

		CreateBoxIndices();
		CreateBoxBuffers(aabb_box);
	}
	if (parent != nullptr)
		UpdateTransform();
}

Mesh::~Mesh()
{
	delete render_data;
}

const RenderData* Mesh::GetRenderData()
{
	return render_data;
}

void Mesh::Update()
{
	if (draw_aabb)
	{
		if (draw_aabb && render_data->aabb_vertex_id > 0)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glLineWidth(2.0f);

			//Bind AABB vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, render_data->aabb_vertex_id);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			//Bind and draw with indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->box_indices_id);
			glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);

			glLineWidth(1.0f);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}

void Mesh::UpdateTransform()
{
	AABB temp = aabb_box;
	if (parent != nullptr) {
		temp.TransformAsAABB(parent->GetTransform()->GetGlobalTransform());
		
	}

	CreateBoxBuffers(temp);

	if (parent != nullptr) parent->boundary_box = temp;
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
	}
}

void Mesh::ChangeMesh()
{
	std::string new_mesh_path;
	if (App->editor->DrawLibraryExplorer(&new_mesh_path))
	{
		delete render_data;
		render_data = App->input->jope_importer.GetNewMesh(new_mesh_path.c_str());

		//Generate AABB/OBB boxes
 		aabb_box.SetNegativeInfinity();
		aabb_box.Enclose((float3*)render_data->vertices, render_data->num_vertices);

		CreateBoxIndices();
		CreateBoxBuffers(aabb_box);
		//if (parent != nullptr) RotateBoundingBox(parent->GetTransform()->GetRotQuat());
		//else LOGC("WARNING: Mesh parent is NULL");

		changing_mesh = false;
	}

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
void Mesh::CreateBoxBuffers(AABB &box)
{
	// delete buffers;
	if (render_data->aabb_vertex_id > 0)
	{
		glDeleteBuffers(1, &render_data->aabb_vertex_id);
	}

	float new_aabb[24]
	{
		box.CornerPoint(0).x, box.CornerPoint(0).y,	box.CornerPoint(0).z,
		box.CornerPoint(1).x, box.CornerPoint(1).y,	box.CornerPoint(1).z,
		box.CornerPoint(2).x, box.CornerPoint(2).y,	box.CornerPoint(2).z,
		box.CornerPoint(3).x, box.CornerPoint(3).y,	box.CornerPoint(3).z,
		box.CornerPoint(4).x, box.CornerPoint(4).y,	box.CornerPoint(4).z,
		box.CornerPoint(5).x, box.CornerPoint(5).y,	box.CornerPoint(5).z,
		box.CornerPoint(6).x, box.CornerPoint(6).y,	box.CornerPoint(6).z,
		box.CornerPoint(7).x, box.CornerPoint(7).y,	box.CornerPoint(7).z,
	};
	
	Primitive temporal_primitive;
	render_data->aabb_vertex_id = temporal_primitive.GenerateBBoxVertices(new_aabb);
}

void Mesh::Save(const char * buffer_data, char * cursor, int& bytes_copied)
{
	LOGC("Saving mesh comp");

	//identifier and type
	int identifier = COMPONENTIDENTIFIER;
	uint bytes_to_copy = sizeof(identifier);
	memcpy(cursor, &identifier, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(type);
	memcpy(cursor, &type, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//UUID and parent UUID
	bytes_to_copy = sizeof(UUID);
	memcpy(cursor, &UUID, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	bytes_to_copy = sizeof(parent_UUID);
	memcpy(cursor, &parent_UUID, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Path to load the mesh
	int name_length = strlen(render_data->mesh_path);
	bytes_to_copy = sizeof(int);
	memcpy(cursor, &name_length, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = name_length;
	memcpy(cursor, render_data->mesh_path, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void Mesh::Load(const char * buffer_data, char * cursor, int & bytes_copied)
{
	//UUID and parentUUID
	uint bytes_to_copy = sizeof(int);
	memcpy(&UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&parent_UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	render_data = new RenderData();
	int name_lenght = 0;
	memcpy(&name_lenght, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = name_lenght;
	render_data->mesh_path = new char[name_lenght];
	render_data->mesh_path[name_lenght] = 0x00;
	memcpy(render_data->mesh_path, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	render_data = App->input->jope_importer.GetNewMesh(render_data->mesh_path);
	//Generate AABB/OBB boxes
	aabb_box.SetNegativeInfinity();
	aabb_box.Enclose((float3*)render_data->vertices, render_data->num_vertices);

	CreateBoxIndices();
	CreateBoxBuffers(aabb_box);
}

void Mesh::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int);			//identifier
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(UUID);
	buffer_size += sizeof(parent_UUID);
	buffer_size += sizeof(int);
	buffer_size += strlen(render_data->mesh_path);
}

RenderData::~RenderData()
{
	if (num_indices > 0)glDeleteBuffers(1, &id_indices);
	if (num_vertices > 0)glDeleteBuffers(1, &id_vertices);
	if (num_normals > 0)glDeleteBuffers(1, &id_normals);
	if (num_tex_vertices > 0)	glDeleteBuffers(1, &id_tex_vertices);
	if (aabb_vertex_id > 0) glDeleteBuffers(1, &aabb_vertex_id);
	if (box_indices_id > 0) glDeleteBuffers(1, &box_indices_id);

	delete[] indices;
	delete[] vertices;
	delete[] normals;
	delete[] tex_vertices;

}
