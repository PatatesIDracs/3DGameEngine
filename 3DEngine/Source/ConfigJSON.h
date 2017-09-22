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

//private: no methods yet
public:
	JSON_Object* conf_object = nullptr;
	JSON_Array*  conf_array = nullptr;
	JSON_Value*	 conf_value = nullptr;

};

#endif // !__CONFIG_JSON__
