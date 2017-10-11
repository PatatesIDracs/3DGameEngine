#include "Material.h"

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
