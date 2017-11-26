#ifndef __PANELABOUT_H__
#define __PANELABOUT_H__

#include "Panel.h"

class PanelAbout : public Panel
{
public:
	PanelAbout();
	~PanelAbout();

	void Draw() override;

};

#endif // !__PANELABOUT_H__

