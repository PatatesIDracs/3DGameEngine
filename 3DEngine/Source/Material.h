#ifndef __MATERIAL_H__
#define	__MATERIAL_H__

#include "Component.h"

class Material : public Component
{
public:
	Material();
	Material(GameObject* parent, int ntexture_id = 0, bool isactive = true);
	~Material();

	int GetTextureID() const;

	void Update();
	void DrawComponent();

private:
	int id_texture = 0;

	uint texture_width = 0;
	uint texture_height = 0;
};

#endif // !__MATERIAL_H__