#include "Resource.h"

Resource::Resource(int uid) : uid(uid), type(RESOURCE_TYPE::RESOURCE_UNKNOW)
{
	SetName(std::to_string(uid));
}

Resource::~Resource()
{
}

int Resource::GetUID()
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

void Resource::GetBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(RESOURCE_TYPE);
	buffer_size += sizeof(int);
	buffer_size += sizeof(uid);
	buffer_size += sizeof(assets_file.c_str());
	buffer_size += sizeof(library_file.c_str());
}

void Resource::SaveResource(std::string * library_path, std::string * assets_path)
{
	uint buffer_size = 0;
	GetBufferSize(buffer_size);
}
