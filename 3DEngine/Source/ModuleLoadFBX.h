#ifndef __MODULE_LOAD_FBX__
#define __MODULE_LOAD_FBX__

#include "Module.h"

struct Mesh_data;

class ModuleLoadFBX : public Module
{
public:
	ModuleLoadFBX(Application* app, bool start_enabled = true);
	~ModuleLoadFBX();

	bool Init();
	bool CleanUp();

	void LoadFile(const char* file_path);

};

#endif // !__MODULE_LOAD_FBX__
