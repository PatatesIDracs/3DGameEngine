#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include "Importer.h"

class MeshImporter : public Importer
{
	MeshImporter();
	~MeshImporter();

	void Import(const char* path);

	void Load(const char* path);

};

#endif // !__IMPORTERMESH_H__
