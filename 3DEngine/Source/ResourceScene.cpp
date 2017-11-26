#include "ResourceScene.h"
#include "GameObject.h"
#include "Component.h"
#include "Application.h"

#include <fstream>

ResourceScene::ResourceScene(int uid) : Resource(uid)
{
	type = RESOURCE_TYPE::RESOURCE_SCENE;
}

ResourceScene::~ResourceScene()
{
}

void ResourceScene::SaveResource(GameObject* scene_root_go)
{
	library_file = std::to_string(uid);
	library_file.append(".jope");

	uint buffer_size = 0;
	GetBufferSize(buffer_size);
	scene_root_go->GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int);

	char* buffer_data = new char[buffer_size];
	char* cursor = buffer_data;

	uint bytes_to_copy = sizeof(RESOURCE_TYPE);
	memcpy(cursor, &type, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(int);		//UID
	memcpy(cursor, &uid, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	int str_size = strlen(name.c_str());
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	bytes_to_copy = str_size; //Resource name
	memcpy(cursor, name.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	str_size = strlen(assets_file.c_str());
	bytes_to_copy = sizeof(int); //Resource assets file
	memcpy(cursor, &str_size, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_to_copy = str_size; //Assets file
	memcpy(cursor, assets_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	str_size = strlen(library_file.c_str());
	bytes_to_copy = sizeof(int); //Resource library file
	memcpy(cursor, &str_size, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_to_copy = str_size; //Library file
	memcpy(cursor, library_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(bool);
	memcpy(cursor, &root_scene, bytes_to_copy);
	cursor += bytes_to_copy;

	int bytes_copied = 0;
	scene_root_go->Save(buffer_data, cursor, bytes_copied);
	cursor += bytes_copied;
	int identifier = ENDFILEIDENTIFIER;
	memcpy(cursor, &identifier, sizeof(identifier));

	std::string save_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER;
	std::ofstream new_file((save_path + library_file).c_str(), std::ofstream::binary);
	new_file.write(buffer_data, buffer_size);

	LOGC("Scene Saved as: %s", library_file.c_str());

	delete[] buffer_data;	
}

void ResourceScene::GetBufferSize(uint & buffer_size)
{
	Resource::GetBufferSize(buffer_size);
	buffer_size += sizeof(bool);
}

void ResourceScene::LoadResourceFromBuffer(char * cursor, int & bytes_copied, uint buffer_size)
{
	//Load the base properties
	Resource::LoadResourceFromBuffer(cursor, bytes_copied, buffer_size);
	cursor += bytes_copied;

	uint bytes_to_copy = sizeof(bool);
	memcpy(&root_scene, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;


	int next_identifier;
	std::vector<GameObject*> loaded_gameobjects;
	std::vector<Component*> loaded_components;

	while (bytes_copied < (int)buffer_size)
	{
		bytes_to_copy = sizeof(int);
		memcpy(&next_identifier, cursor, bytes_to_copy);
		cursor += bytes_to_copy;
		bytes_copied += bytes_to_copy;

		switch (next_identifier)
		{
		case GAMEOBJECTIDENTIFIER:
		{
			GameObject* new_gameobject = new GameObject(nullptr);
			int bytes_advanced = 0;
			new_gameobject->Load(cursor, bytes_advanced);
			cursor += bytes_advanced;
			bytes_copied += bytes_advanced;
			loaded_gameobjects.push_back(new_gameobject);
		}
			break;
		case COMPONENTIDENTIFIER:
		{
			COMP_TYPE new_comp_type = COMP_TYPE::COMP_UNKNOWN;
			bytes_to_copy = sizeof(COMP_TYPE);
			memcpy(&new_comp_type, cursor, bytes_to_copy);
			cursor += bytes_to_copy;
			bytes_copied += bytes_to_copy;

			Component* new_component = App->scene_intro->NewOrphanComponent(new_comp_type);
			int bytes_advanced = 0;
			new_component->Load(cursor, bytes_advanced);
			cursor += bytes_advanced;
			bytes_copied += bytes_advanced;
			loaded_components.push_back(new_component);
		}
			break;
		case ENDFILEIDENTIFIER:
			LOGC("%s scene succesfully loaded", name.c_str());
			break;
		default:
			LOGC("File seems corrupted");
			break;
		}

	}

	for (uint i = 0; i < loaded_components.size(); i++)
	{
		for (uint j = 0; j < loaded_gameobjects.size(); j++)
		{
			if (loaded_gameobjects[j]->GetUUID() == loaded_components[i]->GetParentUUID())
			{
				loaded_gameobjects[j]->AddComponent(loaded_components[i], true);
				if (loaded_components[i]->GetType() == COMP_CAMERA && ((Camera*)loaded_components[i])->IsActive())
					App->camera->SetMainCamera((Camera*)loaded_components[i], true);
				if (loaded_components[i]->GetType() == COMP_MESHRENDERER)
					((MeshRenderer*)loaded_components[i])->PrepareRenderer();
			}
		}
	}

	for (uint i = 0; i < loaded_gameobjects.size(); i++)
	{
		for (uint j = 0; j < loaded_gameobjects.size(); j++)
		{
			if (loaded_gameobjects[j]->GetUUID() == loaded_gameobjects[i]->GetparentUUID())
				loaded_gameobjects[j]->AddChildren(loaded_gameobjects[i]);
		}
	}

	App->scene_intro->LoadGameObjects(&loaded_gameobjects, nullptr, root_scene);
}

//False will be loaded as a prefab, true will be loaded as a new scene in the editor
void ResourceScene::SetAsRoot(bool new_state)
{
	root_scene = new_state;
}
