#ifndef __PANEL_PROPERTIES_H__
#define __PANELPROPERTIES_H__

#include "Panel.h"

class PanelProperties : public Panel
{
public:
	PanelProperties();
	~PanelProperties();

	void Draw() override;

};

#endif // !__PANEL_PROPERTIES_H__