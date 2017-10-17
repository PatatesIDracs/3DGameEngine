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

	void Update();

	void GetElements();

private:
	Transform* transform;
	Mesh* mesh;
	Material* material;

};


#endif // !__MESHRENDERER_H__

