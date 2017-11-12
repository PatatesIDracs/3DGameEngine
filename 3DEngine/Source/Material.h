#ifndef __MATERIAL_H__
#define	__MATERIAL_H__

#include "Component.h"

class ResourceTexture;

class Material : public Component
{
public:
	Material();
	Material(GameObject* parent, ResourceTexture* resource, bool isactive = true);
	~Material();

	int GetTextureID() const;

	void DrawComponent();

	void Save(const char* buffer_data, char* cursor, int& bytes_copied);
	void Load(const char* buffer_data, char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:

	ResourceTexture* resource = nullptr;
};

#endif // !__MATERIAL_H__