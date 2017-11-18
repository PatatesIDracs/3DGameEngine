#include "Resource.h"
#include <fstream>

Resource::Resource(int uid) : uid(uid), type(RESOURCE_TYPE::RESOURCE_UNKNOW)
{
	SetName(std::to_string(uid));
}

Resource::~Resource()
{
}

int Resource::GetUID() const
{
	return uid;
}

void Resource::SetFiles(std::string new_assets_file, std::string new_library_file)
{
	assets_file = new_assets_file;
	library_file = new_library_file;
}

void Resource::SetAssetFile(std::string new_assets_file)
{
	assets_file = new_assets_file;
}

void Resource::SetAssetFile(const char * new_assets_file)
{
	assets_file = new_assets_file;
}

void Resource::SetLibraryFile(std::string new_library_file)
{
	library_file = new_library_file;
}

void Resource::SetLibraryFile(const char * new_library_file)
{
	library_file = new_library_file;
}

void Resource::SetName(const char* new_name)
{
	name = new_name;
}

void Resource::SetName(std::string& new_name)
{
	name = new_name;
}

void Resource::UseThis()
{
	if (loaded == 0) {
		LoadToMemory();
	}
	loaded++;
}

void Resource::LoadToMemory()
{
}

void Resource::StopThis()
{
	loaded--;
	if (loaded == 0) {
		UnloadFromMemory();
	}
}

void Resource::UnloadFromMemory() {
}

const char* Resource::GetName() const
{
	return name.c_str();
}

const char * Resource::GetAssetsPath() const
{
	return assets_file.c_str();
}

const char * Resource::GetLibraryPath() const
{
	return library_file.c_str();
}

void Resource::GetBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(RESOURCE_TYPE);
	buffer_size += sizeof(int) * 4;			//uid + 3 string lengths
	buffer_size += strlen(name.c_str());
	buffer_size += strlen(assets_file.c_str());
	buffer_size += strlen(library_file.c_str());
}

void Resource::SaveResource()
{
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
	int str_size = strlen(name.c_str());
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
	bytes_to_copy = str_size; //Assets file
	memcpy(cursor, assets_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	//Save library_file string
	str_size = strlen(library_file.c_str());
	bytes_to_copy = sizeof(int); //Resource name
	memcpy(cursor, &str_size, bytes_to_copy);
	bytes_to_copy = str_size; //Library file
	memcpy(cursor, library_file.c_str(), bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor
}

//Load the library file to a buffer and pass it to the LoadResourceFromBuffer method
void Resource::LoadResource()
{
	LOGC("Loading Resource from %s", library_file.c_str());

	char* buffer_data = nullptr;
	uint buffer_size = 0;
	std::ifstream loaded_file(library_file.c_str(), std::fstream::binary);
	if (loaded_file)
	{
		loaded_file.seekg(0, loaded_file.end);
		buffer_size = loaded_file.tellg();
		loaded_file.seekg(0, loaded_file.beg);

		buffer_data = new char[buffer_size];

		loaded_file.read(buffer_data, buffer_size);
		loaded_file.close();
	}
	if (buffer_data == nullptr)
	{
		LOGC("File %s couldn't be loaded from library", library_file.c_str());
		return;
	}
	char* cursor = buffer_data;
	int bytes_copied;

	LoadResourceFromBuffer(cursor, bytes_copied, buffer_size);
}

void Resource::LoadResourceFromBuffer(char* cursor, int& bytes_copied, uint buffer_size)
{
	//Type load
	uint bytes_to_copy = sizeof(RESOURCE_TYPE);
	memcpy(&type, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//Uid load
	bytes_to_copy = sizeof(int);
	memcpy(&uid, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;

	//load name size
	int str_size = 0;
	memcpy(&str_size, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	//load the name string
	char* name_c_str = new char[str_size + 1];
	bytes_to_copy = str_size;
	memcpy(name_c_str, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	name_c_str[str_size] = 0x00;
	name = name_c_str;
	delete[] name_c_str;


	//load assets_file size
	bytes_to_copy = sizeof(int);
	memcpy(&str_size, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	//load the assets_file string
	char* assets_file_c_str = new char[str_size + 1];
	bytes_to_copy = str_size;
	memcpy(assets_file_c_str, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	assets_file_c_str[str_size] = 0x00;
	name = assets_file_c_str;
	delete[] assets_file_c_str;


	//load library_file size
	bytes_to_copy = sizeof(int);
	memcpy(&str_size, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	//load the library_file string
	char* library_file_c_str = new char[str_size + 1];
	bytes_to_copy = str_size;
	memcpy(library_file_c_str, cursor, bytes_to_copy);
	cursor += bytes_to_copy;
	bytes_copied += bytes_to_copy;
	library_file_c_str[str_size] = 0x00;
	name = library_file_c_str;
	delete[] library_file_c_str;

}
