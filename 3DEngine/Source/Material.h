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
	int GetResourceLoaded() const;

	void DrawComponent();

	void ChangeTexture();
	void Save(const char* buffer_data, char* cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:

	ResourceTexture* resource = nullptr;
	int resource_uid = 0;

	bool changing_text = false;
};

#endif // !__MATERIAL_H__