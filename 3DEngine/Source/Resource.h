#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include "Globals.h"
#include <string>

enum RESOURCE_TYPE
{
	RESOURCE_UNKNOW = 0,
	RESOURCE_MESH,
	RESOURCE_TEXTURE,
	RESOURCE_SCENE
};

class Resource
{
public:
	Resource(int uid);
	virtual ~Resource();

	int GetUID() const;
	RESOURCE_TYPE GetType()const;

	void SetFiles(std::string new_assets_file, std::string new_library_file);
	void SetAssetFile(std::string new_assets_file);
	void SetAssetFile(const char* new_assets_file);
	void SetLibraryFile(std::string new_library_file);
	void SetLibraryFile(const char* new_library_file);

	void SetName(const char* new_name);
	void SetName(std::string& new_name);

	void UseThis();
	virtual void LoadToMemory();
	void StopThis();
	virtual void UnloadFromMemory();
	virtual void ResourceModified();

	const char* GetName() const;
	const char* GetAssetsPath() const;
	const char* GetLibraryPath() const;
	uint			GetTimesLoaded() const;
	virtual void GetBufferSize(uint& buffer_size);
	virtual void SaveResource();
	virtual void LoadResource();
	virtual void LoadResourceFromBuffer(char* cursor, int& bytes_copied, uint buffer_size);

protected:
	RESOURCE_TYPE type = RESOURCE_UNKNOW;
	int uid = 0;
	uint loaded = 0;
	std::string name;				//Resource name displayed in the editor, different from the file name
	std::string assets_file;
	std::string library_file;

};


#endif // !__RESOURCES_H__