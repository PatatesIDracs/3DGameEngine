#include "ResourceScene.h"
#include "GameObject.h"

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
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	bytes_to_copy = str_size; //Assets file
	memcpy(cursor, assets_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	str_size = strlen(library_file.c_str());
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	bytes_to_copy = str_size; //Library file
	memcpy(cursor, library_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

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
}

void ResourceScene::LoadResource(char * cursor, int & bytes_copied)
{

}
