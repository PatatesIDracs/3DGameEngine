#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include <string>
#include <map>

#define MESHFILEFORMAT ".mjope"

struct RenderData;
class GameObject;
struct aiNode;
struct aiScene;
template <class T>
class aiMatrix4x4t;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	void Import(const char* full_path, std::string& path, std::string& file_name, std::string& extension, GameObject* import_target);

	RenderData* Load(const char* full_path);

private:

	const char* SaveMesh(RenderData* mesh, const char* file_name);

	void ImportNode(aiNode* to_import, const aiScene* scene, GameObject* import_target);
	void ImportNodeChild(aiNode* to_import, const aiScene* scene, GameObject* import_target, aiMatrix4x4t<float> parent_transform);

	//Return a map with assimpID/ResourceUID
	std::map<int, int>* ImportMeshResources(const aiScene* scene, std::string& file_name);

private:
	std::string import_path;

};

#endif // !__IMPORTERMESH_H__
