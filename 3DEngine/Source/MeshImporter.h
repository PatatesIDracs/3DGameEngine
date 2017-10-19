#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include <string>

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	void Import(std::string* path, std::string* filename, std::string* extension);


};

#endif // !__IMPORTERMESH_H__
