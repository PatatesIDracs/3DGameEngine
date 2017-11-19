#include "ResourceTexture.h"
#include <fstream>

#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

ResourceTexture::ResourceTexture(int uid): Resource(uid)
{
	type = RESOURCE_TYPE::RESOURCE_TEXTURE;
}

void ResourceTexture::LoadToMemory()
{
	if (texture_id == 0 && width > 1 && library_file.size() >0) {
		texture_id = ilutGLLoadImage((char*)(JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER + library_file).c_str());
	}
}

void ResourceTexture::UnloadFromMemory()
{
	if (texture_id != 0)glDeleteTextures(1, &texture_id);
}

void ResourceTexture::SaveResource()
{
	library_file = JOPE_TEXTURE_FOLDER + std::to_string(uid);
	library_file.append(TEXFORMAT);

	uint buffer_size = 0;
	GetBufferSize(buffer_size);

	char* buffer_data = new char[buffer_size];
	char* cursor = buffer_data;

	uint bytes_to_copy = sizeof(RESOURCE_TYPE);
	memcpy(cursor, &type, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(int);		//UID
	memcpy(cursor, &uid, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	//Save name string
	int str_size = name.size();
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	bytes_to_copy = str_size; //Resource name
	memcpy(cursor, name.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	//Save assets_file string
	str_size = strlen(assets_file.c_str());
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	bytes_to_copy = str_size; //Assets file
	memcpy(cursor, assets_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	//Save library_file string
	str_size = strlen(library_file.c_str());
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	bytes_to_copy = str_size; //Library file
	memcpy(cursor, library_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor


	bytes_to_copy = sizeof(uint);
	memcpy(cursor, &width, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	memcpy(cursor, &height, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	memcpy(cursor, &depth, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	memcpy(cursor, &mips, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	memcpy(cursor, &bytes, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	memcpy(cursor, &texture_id, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(FORMAT);
	memcpy(cursor, &format, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	//Save dds path
	str_size = strlen(path_to_dds.c_str());
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
	bytes_to_copy = str_size; //Library file
	memcpy(cursor, path_to_dds.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	//Write the file
	std::string save_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER;
	std::ofstream new_file((save_path + library_file).c_str(), std::ofstream::binary);
	new_file.write(buffer_data, buffer_size);

	LOGC("Textures Saved at: %s", library_file.c_str());

	delete[] buffer_data;
}

void ResourceTexture::LoadResourceFromBuffer(char * cursor, int & bytes_copied, uint buffer_size)
{
	Resource::LoadResourceFromBuffer(cursor, bytes_copied, buffer_size);
	cursor += bytes_copied;

	uint bytes_to_copy = sizeof(uint);
	memcpy(&width, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&height, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&depth, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&mips, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&bytes, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	memcpy(&texture_id, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	
	//format
	bytes_to_copy = sizeof(FORMAT);
	memcpy(&format, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//String to dds
	bytes_to_copy = sizeof(int);
	int str_size = 0;
	memcpy(&str_size, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	char* dds_path_str = new char[str_size + 1];
	bytes_to_copy = str_size;
	memcpy(dds_path_str, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	dds_path_str[str_size] = 0x00;
	path_to_dds = dds_path_str;
	delete[] dds_path_str;
}

void ResourceTexture::GetBufferSize(uint & buffer_size)
{
	Resource::GetBufferSize(buffer_size);

	buffer_size += sizeof(uint) * 6;
	buffer_size += sizeof(FORMAT);
	buffer_size += sizeof(int);
	buffer_size += strlen(path_to_dds.c_str());
}

uint ResourceTexture::GetTextureID() const
{
	return texture_id;
}
