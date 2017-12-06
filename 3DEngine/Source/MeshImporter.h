#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include <string>
#include <map>

class GameObject;
class ResourceMesh;
class ResourceScene;
class ResourcePrefab;
class Config_Json;
template <class T>
class aiMatrix4x4t;
struct aiNode;
struct aiScene;
struct RenderData;

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	void Import(const char* full_path, std::string& path, std::string& file_name, std::string& extension);

	RenderData* Load(const char* full_path);

	void GetAssimpVersion(int& major, int& minor, int& revision);

private:

	const char* SaveMesh(RenderData* mesh, const char* file_name);

	void ImportScene(const aiScene* scene, std::map<int, int>* id_map, std::map<int, int>* text_map, std::string& file_name, const char* full_path);
	void ImportNode(aiNode* to_import, const aiScene* scene, GameObject* import_target, aiMatrix4x4t<float> parent_transform, std::map<int, int>* id_map, std::map<int, int>* text_map);

	//Return a map with assimpID/ResourceUID
	std::map<int, int>* ImportMeshResources(const aiScene* scene, std::string& file_name);

	// Retrun a map with assimpID/ResourceUID Texture
	std::map<int, int>* ImportTextureResources(const aiScene* scene, const char * full_path);

	void WriteSceneMeta(Config_Json& meta_file, const ResourcePrefab* resource) const;
	void WriteMeshMeta(Config_Json& meta_file, const ResourceMesh* resource) const;
	
private:
	std::string import_path;
	std::string assets_meshes_path;

};

#endif // !__IMPORTERMESH_H__
