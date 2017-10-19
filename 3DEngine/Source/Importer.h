#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include <string>

class MeshImporter;

class Importer
{
public:
	Importer();
	~Importer();

	void Import(char* full_path);

	void DividePath(char* full_path, std::string* path, std::string* filename, std::string* extension);
	

private:

	//Specialized importers
	MeshImporter* mesh_importer = nullptr;
};

#endif // !__IMPORTER_H__