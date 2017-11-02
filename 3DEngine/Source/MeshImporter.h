#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include <string>

#define MESHFILEFORMAT ".mjope"

class RenderData;
class GameObject;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	void Import(const char* full_path, GameObject* import_target);

	RenderData* Load(const char* full_path);

private:

	void SaveMesh(RenderData* mesh, const char* file_name);


private:
	std::string import_path;

};

#endif // !__IMPORTERMESH_H__
