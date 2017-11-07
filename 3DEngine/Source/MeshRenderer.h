#ifndef __MESHRENDERER_H__
#define __MESHRENDERER_H__

#include "Component.h"

class Mesh;
class Transform;
class Material;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* parent);
	~MeshRenderer();

	//Get the elements to render from parent
	void PrepareRenderer();

	void DrawComponent();
	void ChangeParent(GameObject* new_parent);

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(const char * buffer_data, char * cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

public:
	Transform* transform;
	Mesh* mesh;
	Material* material;

};


#endif // !__MESHRENDERER_H__

