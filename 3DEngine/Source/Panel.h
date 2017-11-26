#ifndef __PANEL_H__
#define __PANEL_H__

class Panel
{

public:
	Panel();
	virtual ~Panel();

	virtual void Draw();

	bool IsActive() const;

protected:
	bool active = false;
};

#endif // !__PANEL_H__
