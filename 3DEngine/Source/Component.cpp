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
		parent_UUID = parent->GetUUID();
}

void Component::GetOwnBufferSize(uint & buffer_size)
{
	buffer_size += sizeof(int) * 3;		//identifier + UUID + parentUUID
	buffer_size += sizeof(COMP_TYPE);
	buffer_size += sizeof(bool) * 2;	//active + unique
}

const char * Component::TypeToString()
{
	switch (type)
	{
	case COMP_UNKNOWN:
		return "Unknown";
		break;
	case COMP_TRANSFORM:
		return "Transform";
		break;
	case COMP_MESH:
		return "Mesh";
		break;
	case COMP_MESHRENDERER:
		return "Mesh Renderer";
		break;
	case COMP_CAMERA:
		return "Camera";
		break;
	case END_UNIQUE_COMP:
		break;
	case COMP_MATERIAL:
		return "Material";
		break;
	default:
		break;
	}

	return "Unknown";
}

bool Component::IsUnique() const
{
	if (type < END_UNIQUE_COMP)
		return true;
	else
		return false;
}


void Component::ReloadUUID()
{
	UUID = App->GetIntUUID();
	if (parent != nullptr)
		parent_UUID = parent->GetUUID();
	else
		parent_UUID = 0;
}
