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
	{
		resource->UseThis();
		resource_uid = resource->GetUID();
	}
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

int Material::GetResourceLoaded() const
{
	if (resource) {
		return resource->GetTimesLoaded();
	}
	return -1;
}

void Material::DrawComponent()
{
	ImGui::PushID(UUID);
	if (ImGui::CollapsingHeader("Texture"))
	{
		if (ImGui::Button("Change")) changing_text = !changing_text;

		if (changing_text) ChangeTexture();

		if (resource != nullptr)
		{
			ImGui::InputInt("Width:", (int*)&resource->width, 0, 100, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Height:", (int*)&resource->height, 0, 100, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Bpp:", (int*)&resource->bytes, 0, 100, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Depth:", (int*)&resource->depth, 0, 100, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Mips:", (int*)&resource->mips, 0, 100, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("ID:", (int*)&resource->texture_id, 0, 100, ImGuiInputTextFlags_ReadOnly);
			ImGui::Image((void*)resource->texture_id, ImVec2(200.f, 200.f), ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			ImGui::Text("No texture found");
		}
	}
	ImGui::PopID();
}

void Material::ChangeTexture()
{
	ResourceTexture* new_text = (ResourceTexture*)App->editor->DrawResourceExplorer(RESOURCE_TEXTURE, "Texture Folder");

	if (new_text)
	{
		if (resource != nullptr) {
			resource->StopThis();
		}
		resource = new_text;
		resource->UseThis();
		resource_uid = resource->GetUID();
		changing_text = false;
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
	memcpy(cursor, &resource_uid, bytes_to_copy);
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
	memcpy(&resource_uid, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	resource = (ResourceTexture*)App->resources->GetFromUID(resource_uid);
	if (resource != nullptr)
		resource->UseThis();
}

void Material::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int); //Resource id
}
