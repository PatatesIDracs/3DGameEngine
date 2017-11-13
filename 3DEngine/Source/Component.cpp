#include "Component.h"
#include "Application.h"

Component::Component() : parent(nullptr), type(COMP_UNKNOWN), active(false)
{
	UUID = App->GetIntUUID();
}

Component::Component(GameObject * parent, COMP_TYPE type, bool isactive) : parent(parent), type(type), active(isactive)
{
	LCG UUIDGen;
	UUID = UUIDGen.Int();
	if (parent != nullptr)
		parent_UUID = parent->UUID;
}

void Component::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int) * 3;		//identifier + UUID + parentUUID
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(bool) * 2;	//active + unique
};
