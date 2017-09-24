#ifndef __CONFIG_JSON__
#define __CONFIG_JSON__

#include "Globals.h"
#include <math.h>

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class Config_Json
{
public:
	Config_Json();
	Config_Json(const char* filename);
	~Config_Json();

	// Get Methods
	int			GetInt(const char* name, int default = 0) const;
	float		GetFloat(const char* name, float default = 0.0f) const;
	const char* GetString(const char* name, const char* default = "") const;
	bool		GetBool(const char* name, bool default = false) const;
	// Dot Get Methods
	int			GetDotInt(const char* name, int default = 0) const;
	float		GetDotFloat(const char* name, float default = 0.0f) const;
	const char* GetDotString(const char* name, const char* default = "") const;
	bool		GetDotBool(const char* name, bool default = false) const;
	
	// Set Methods
	bool		SetInt(const char* name, int value);
	bool		SetFloat(const char* name, float value);
	bool		SetString(const char* name, const char* value);
	bool		SetBool(const char* name, bool value);
	// Dot Set Methods
	bool		SetDotInt(const char* name, int value);
	bool		SetDotFloat(const char* name, float value);
	bool		SetDotString(const char* name, const char* value);
	bool		SetDotBool(const char* name, bool value);
	
	// Serialize data to file and CleanUp
	void		SaveToFile(const char* filename);

//private: no methods yet
public:
	JSON_Object* config_obj = nullptr;
	JSON_Array*  config_array = nullptr;
	JSON_Value*	 config_val = nullptr;

};

#endif // !__CONFIG_JSON__
