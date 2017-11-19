#include "Material.h"
#include "Glew\include\glew.h"
#include "Application.h"
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
	if (resource != nullptr)
		resource->StopThis();
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

	bytes_to_copy = sizeof(int);
	memcpy(cursor, &resource_id, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;


}

void Material::Load(char * cursor, int & bytes_copied)
{
	//UUID and parentUUID
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
	bytes_to_copy = sizeof(int);
	memcpy(&resource_id, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	resource = (ResourceTexture*)App->resources->GetFromUID(resource_id);
}

void Material::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int); //Resource id
}
