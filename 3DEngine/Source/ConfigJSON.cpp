#include "ConfigJSON.h"

#include "parson.h"
#include <stdio.h>
#include <stdlib.h>


Config_Json::Config_Json()
{
	conf_value = nullptr;

}

Config_Json::Config_Json(const char * filename)
{
	conf_value = json_parse_file(filename);
	if (conf_value != NULL)
	{
		conf_object = json_value_get_object(conf_value);
	}
}

Config_Json::~Config_Json()
{
	if (conf_value != nullptr)
	{
		delete conf_value;
	}
	if (conf_object != nullptr)
	{
		delete conf_object;
	}
}

// Get Methods -----------------------------------

int Config_Json::GetInt(const char * name, int default) const
{
	JSON_Value* value = json_object_get_value(conf_object, name);

	if (value && json_value_get_type(value) == JSONNumber)
		return (int)json_value_get_number(value);
	
	return default;
}

float Config_Json::GetFloat(const char* name, float default) const
{
	JSON_Value* value = json_object_get_value(conf_object, name);

	if (value && json_value_get_type(value) == JSONNumber)
		return (float)json_value_get_number(value);
	
	return default;
}

const char* Config_Json::GetString(const char* name, const char* default) const 
{
	JSON_Value* value = json_object_get_value(conf_object, name);

	if (value && json_value_get_type(value) == JSONString)
		return json_value_get_string(value);
	
	return default;
}

bool Config_Json::GetBool(const char* name, bool default) const 
{
	JSON_Value* value = json_object_get_value(conf_object, name);

	if (value && json_value_get_type(value) == JSONBoolean)
		return (bool)json_value_get_boolean;

	return default;	
}
