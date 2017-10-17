#include "Material.h"
#include "Glew\include\glew.h"

Material::Material()
{
}

Material::Material(GameObject * parent, float3 texture, bool isactive) : Component(parent, COMP_MATERIAL, isactive), id_texture(texture.x),
texture_width(texture.y), texture_height(texture.z)
{
}

Material::~Material()
{
}

int Material::GetTextureID() const
{
	return id_texture;
}

void Material::Update()
{
	glBindTexture(GL_TEXTURE_2D, id_texture);
}

void Material::DrawComponent()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::InputInt("Width:", (int*)&texture_width, 0, 100, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputInt("Height:", (int*)&texture_height, 0, 100, ImGuiInputTextFlags_ReadOnly);
	}
}
