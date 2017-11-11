#include "Resource.h"

Resource::Resource(int uid) : uid(uid), type(RESOURCE_TYPE::RESOURCE_UNKNOW)
{
}

Resource::~Resource()
{
}

int Resource::GetUID()
{
	return uid;
}
