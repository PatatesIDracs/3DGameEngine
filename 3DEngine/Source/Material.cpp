#include "Material.h"
#include "Glew\include\glew.h"

#include "Resource.h"
#include "ResourceTexture.h"

Material::Material()
{
}

Material::Material(GameObject * parent, ResourceTexture* resource, bool isactive) : Component(parent, COMP_MATERIAL, isactive), resource(resource)
{
	if (parent != nullptr)
		parent->AddComponent(this);

	if (resource != nullptr)
		resource->UseThis();
}

Material::~Material()
{
}

int Material::GetTextureID() const
{
	if(resource != nullptr)
		return resource->GetTextureID();
	else return 0;
}

void Material::DrawComponent()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::InputInt("Width:", (int*)&resource->width, 0, 100, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputInt("Height:", (int*)&resource->height, 0, 100, ImGuiInputTextFlags_ReadOnly);
	}
}

void Material::Save(const char * buffer_data, char * cursor, int& bytes_copied)
{
	/*
	LOGC("Saving material comp");

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

	bytes_to_copy = sizeof(id_texture);
	memcpy(cursor, &id_texture, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	bytes_to_copy = sizeof(uint);
	memcpy(cursor, &texture_width, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(cursor, &texture_height, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;*/
}

void Material::Load(const char * buffer_data, char * cursor, int & bytes_copied)
{
	/*//UUID and parentUUID
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

	//Texture id
	memcpy(&id_texture, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//height and width
	bytes_to_copy = sizeof(uint);
	memcpy(&texture_width, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&texture_height, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;*/
}

void Material::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);

	//buffer_size += sizeof(id_texture);
	buffer_size += sizeof(uint) * 2;	//Texture height and width
}
