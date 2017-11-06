#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Glew\include\glew.h"

MeshRenderer::MeshRenderer(GameObject* parent) : Component(parent, COMP_MESHRENDERER)
{
	if (parent != nullptr)
		GetElements();
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::GetElements()
{
	transform = (Transform*)parent->GetTransform();
	mesh = (Mesh*)parent->FindFirstComponent(COMP_MESH);
	material = (Material*)parent->FindFirstComponent(COMP_MATERIAL);
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
}

void MeshRenderer::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int);			//identifier
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(UUID);
	buffer_size += sizeof(parent_UUID);
}
