#ifndef __IMPORTER_H__
#define __IMPORTER_H__

class Importer
{
public:
	Importer() {};
	~Importer() {};

	virtual void Import(const char* path) 
	{};
	virtual void Load(const char* path) 
	{};

};

#endif // !__IMPORTER_H__