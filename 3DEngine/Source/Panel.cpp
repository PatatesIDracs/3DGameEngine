#include "Panel.h"

Panel::Panel()
{
	name = "DefaultPanel";
}

Panel::~Panel()
{
}

void Panel::Draw()
{
}

void Panel::ChangeState()
{
	active = !active;
}

bool Panel::IsActive() const
{
	return active;
}

const char * Panel::GetName() const
{
	return name.c_str();
}
