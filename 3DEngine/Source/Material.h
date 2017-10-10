#ifndef __MATERIAL_H__
#define	__MATERIAL_H__

#include "Component.h"

class Material : public Component
{
public:
	Material();
	Material(GameObject* parent, bool isactive = true);
	~Material();

};

#endif // !__MATERIAL_H__