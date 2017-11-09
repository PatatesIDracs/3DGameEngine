#ifndef __RESOURCESCENE_H__
#define __RESOURCESCENE_H__

#include "Resource.h"

class ResourceScene : public Resource
{
public:
	ResourceScene(int uid, RESOURCE_TYPE type);
	~ResourceScene();

};

#endif // !__RESOURCESCENE_H__

