#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Glew\include\glew.h"

MeshRenderer::MeshRenderer(GameObject* parent) : Component(parent, COMP_MESHRENDERER)
{
	if (parent != nullptr) {
		parent->AddComponent(this);
		PrepareRenderer();
	}
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::PrepareRenderer()
{
	transform = (Transform*)parent->GetTransform();
	mesh = (Mesh*)parent->FindFirstComponent(COMP_MESH);
	material = (Material*)parent->FindFirstComponent(COMP_MATERIAL);
}

void MeshRenderer::DrawComponent()
{
	ImGui::PushID(UUID);
	Component::DrawComponent();
	if (ImGui::CollapsingHeader("Mesh Renderer"))
	{
		if (mesh == nullptr)
			ImGui::Text("No mesh");
		else
			ImGui::Text("Some mesh"); //TODO: name of the mesh

		if (ImGui::Button("Reload elements")) PrepareRenderer();
	}
	ImGui::PopID();
}

void MeshRenderer::ChangeParent(GameObject * new_parent)
{
	parent = new_parent;
	parent_UUID = new_parent->GetUUID();
	PrepareRenderer();
}

void MeshRenderer::Save(const char * buffer_data, char * cursor, int& bytes_copied)
{
	LOGC("Saving meshRenderer comp");

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

	//active and unique
	bytes_to_copy = sizeof(bool);
	memcpy(cursor, &active, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &unique, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
}

void MeshRenderer::Load(char * cursor, int & bytes_copied)
{
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
	
	LOGC("Component mesh renderer loaded");
}

void MeshRenderer::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
}
