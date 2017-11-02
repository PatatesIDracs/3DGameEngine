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
	mesh = (Mesh*)parent->FindUniqueComponent(COMP_MESH);
	material = (Material*)parent->FindUniqueComponent(COMP_MATERIAL);
}
