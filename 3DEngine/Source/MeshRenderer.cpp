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

void MeshRenderer::GetElements()
{
	transform = (Transform*)parent->GetTransform();
	mesh = (Mesh*)parent->FindFirstComponent(COMP_MESH);
	material = (Material*)parent->FindFirstComponent(COMP_MATERIAL);
}

void MeshRenderer::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(UUID);
	buffer_size += sizeof(parent_UUID);
}
