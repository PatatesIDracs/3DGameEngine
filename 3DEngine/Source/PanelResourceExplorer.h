#ifndef __PANELRESOURCEEXPLORER_H__
#define __PANELRESOURCEEXPLORER_H__

#include "Panel.h"
#include "Resource.h"
#include <vector>

class PanelResourceExplorer : public Panel
{
public:
	PanelResourceExplorer();
	~PanelResourceExplorer();

	void Draw() override;
	void ChangeState();
	void SetTypeToShow(RESOURCE_TYPE new_type_to_show);

	Resource* GetLoadedResource() const;

private:
	const char* ResourceTypeToString(RESOURCE_TYPE type);
	void LoadResourcesArray();

private:

	std::vector<Resource*>* to_show = nullptr;

	//Resource unknow == all resources
	RESOURCE_TYPE type_to_show = RESOURCE_TYPE::RESOURCE_UNKNOW; 

	Resource* to_load = nullptr;
};


#endif // !__PANELRESOURCEEXPLORER_H__