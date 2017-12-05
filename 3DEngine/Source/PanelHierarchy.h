#ifndef __PANELHIERARCHY_H__
#define __PANELHIERARCHY_H__

#include "Panel.h"

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();
	~PanelHierarchy();

	void Draw() override;

private:


};

#endif // !__PANELHIERARCHY_H__