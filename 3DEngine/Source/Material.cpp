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
