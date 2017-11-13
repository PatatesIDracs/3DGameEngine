#include "Mesh.h"
#include "Glew\include\glew.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "Transform.h"

#include "ResourceMesh.h"

Mesh::Mesh(GameObject* parent, ResourceMesh* mesh_resource, bool isactive) : Component(parent, COMP_MESH, isactive), mesh_resource(mesh_resource)
{
	if (parent != nullptr) {
		parent->AddComponent(this);
		if (mesh_resource != nullptr) {
			mesh_resource->UseThis();
			UpdateTransform();
		}
	}
}

Mesh::~Mesh()
{
	if (mesh_resource != nullptr)
		mesh_resource->StopThis();
}

const RenderData* Mesh::GetRenderData() const
{
	return mesh_resource->GetRenderData();
}

void Mesh::Update()
{
	if (draw_aabb)
	{
		parent->GetBoundaryBox().Draw(3.0f, float4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

void Mesh::UpdateTransform()
{
	parent->boundary_box = mesh_resource->GetAABB();
	parent->boundary_box.TransformAsAABB(parent->GetTransform()->GetGlobalTransform());
}

void Mesh::DrawComponent()
{
	if (ImGui::CollapsingHeader("Geometry"))
	{
		ImGui::Text("%s", mesh_resource->GetName()); ImGui::SameLine();
		if (ImGui::Button("Change")) changing_mesh = !changing_mesh;
		if (changing_mesh) ChangeMesh();

		ImGui::InputInt("Vertices", (int*)&GetRenderData()->num_vertices, 0, 100, ImGuiInputTextFlags_ReadOnly);

		int faces = GetRenderData()->num_indices / 3;
		ImGui::InputInt("Faces", &faces, 0, 100, ImGuiInputTextFlags_ReadOnly);

		ImGui::Checkbox("Draw AABB", &draw_aabb);

	}
}

bool Mesh::CheckRayCollision(const LineSegment segment, float & dist, float3 & point)
{
	if (mesh_resource == nullptr) return false;

	bool ret = false;
	Ray ray = segment.ToRay();
	ray.Transform(parent->GetTransform()->GetGlobalTransform().Inverted());
	
	const RenderData* mesh_data = mesh_resource->GetRenderData();

	Triangle tri;

	float best_dist = dist;
	float3 intersect_point;

	for (uint i = 0; i < mesh_data->num_indices; i += 3)
	{
		tri.a = float3(&mesh_data->vertices[mesh_data->indices[i] * 3]);
		tri.b = float3(&mesh_data->vertices[mesh_data->indices[i + 1] * 3]);
		tri.c = float3(&mesh_data->vertices[mesh_data->indices[i + 2] * 3]);

		if (ray.Intersects(tri, &best_dist, &intersect_point)) {
			if (dist == -1 || best_dist < dist)
			{
				dist = best_dist;
				point = intersect_point;
				ret = true;
			}
		}
	}

	return ret;
}

void Mesh::ChangeMesh()
{
	std::string new_mesh_path;
	if (App->editor->DrawFixedExplorer(new_mesh_path, JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER))
	{
		// mesh_resource = App->resource->GetResource....
		
		UpdateTransform();
		changing_mesh = false;
	}

}

void Mesh::Save(const char * buffer_data, char * cursor, int& bytes_copied)
{
	LOGC("Saving mesh comp");

/*	//identifier and type
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

	//active and unique
	bytes_to_copy = sizeof(bool);
	memcpy(cursor, &active, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &unique, bytes_to_copy);
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
	*/
}

void Mesh::Load(const char * buffer_data, char * cursor, int & bytes_copied)
{
	/*
	//UUID and parentUUID
	uint bytes_to_copy = sizeof(int);
	memcpy(&UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&parent_UUID, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//active and unique
	bytes_to_copy = sizeof(bool);
	memcpy(&active, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&unique, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Load mesh.mjope path
	render_data = new RenderData;
	int name_lenght = 0;
	bytes_to_copy = sizeof(int);
	memcpy(&name_lenght, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = name_lenght;
	render_data->mesh_path = new char[name_lenght];
	render_data->mesh_path[name_lenght] = 0x00;
	memcpy(render_data->mesh_path, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

//	render_data = App->input->jope_importer.GetNewMesh(render_data->mesh_path);
	if (render_data != nullptr)
	{
		//Generate AABB/OBB boxes
		aabb_box.SetNegativeInfinity();
		aabb_box.Enclose((float3*)render_data->vertices, render_data->num_vertices);

		CreateBoxIndices();
		CreateBoxBuffers(aabb_box);
	}
	else
		LOGC("Error: Unable to load the mesh, file not found in library please import again");

		*/
}

void Mesh::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);

	buffer_size += sizeof(int);
//	buffer_size += strlen(render_data->mesh_path);
}