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
