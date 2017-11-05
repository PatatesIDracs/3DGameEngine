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

void Material::DrawComponent()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::InputInt("Width:", (int*)&texture_width, 0, 100, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputInt("Height:", (int*)&texture_height, 0, 100, ImGuiInputTextFlags_ReadOnly);
	}
}

void Material::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(UUID);
	buffer_size += sizeof(parent_UUID);
	buffer_size += sizeof(id_texture);
	buffer_size += sizeof(uint) * 2;	//Texture height and width
}
