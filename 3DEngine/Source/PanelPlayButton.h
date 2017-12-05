#ifndef __PANELPLAYBUTTON_H__
#define __PANELPLAYBUTTON_H__

#include "Panel.h"

class PanelPlayButton : public Panel
{
public:
	PanelPlayButton();
	~PanelPlayButton();

	void Draw() override;

private:
	void DrawClock();

private:
	bool showclock = false;
};

#endif // !__PANELPLAYBUTTON_H__