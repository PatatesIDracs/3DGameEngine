#include "Material.h"

Material::Material()
{
}

Material::Material(GameObject * parent, bool isactive) : Component(parent, COMP_MATERIAL, isactive)
{
}

Material::~Material()
{
}
