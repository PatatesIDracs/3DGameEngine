#include "Material.h"
#include "Glew\include\glew.h"

Material::Material()
{
}

Material::Material(GameObject * parent, int ntexture_id, bool isactive) : Component(parent, COMP_MATERIAL, isactive), id_texture(ntexture_id)
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
