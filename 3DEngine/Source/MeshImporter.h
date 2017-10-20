#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include <string>

#define MESHFILEFORMAT ".mjope"

class MeshImporter
{
public:
	MeshImporter();
	~MeshImporter();

	void Import(const char* full_path);

};

#endif // !__IMPORTERMESH_H__
