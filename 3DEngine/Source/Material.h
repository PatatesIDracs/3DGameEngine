#ifndef __MATERIAL_H__
#define	__MATERIAL_H__

#include "Component.h"

class Material : public Component
{
public:
	Material();
	Material(GameObject* parent, float3 ntexture_id = float3(0,0,0), bool isactive = true);
	~Material();

	int GetTextureID() const;

	void DrawComponent();

	void Save(const char* buffer_data, char* cursor, int& bytes_copied);
	void Load(const char* buffer_data, char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:
	int id_texture = 0;

	uint texture_width = 0;
	uint texture_height = 0;
};

#endif // !__MATERIAL_H__