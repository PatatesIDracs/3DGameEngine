#include "ConfigJSON.h"

#include "parson.h"
#include <stdio.h>
#include <stdlib.h>


Config_Json::Config_Json()
{
	config_val = json_value_init_object();
	config_obj = json_value_get_object(config_val);
}

Config_Json::Config_Json(const char * filename)
{
	config_val = json_parse_file(filename);
	if (config_val != NULL)
	{
		config_obj = json_value_get_object(config_val);
	}
}

Config_Json::~Config_Json()
{
}

// Get Methods -----------------------------------

int Config_Json::GetInt(const char * name, int default) const
{
	JSON_Value* value = json_object_get_value(config_obj, name);

	if (value && json_value_get_type(value) == JSONNumber)
		return (int)json_value_get_number(value);
	
	return default;
}

float Config_Json::GetFloat(const char* name, float default) const
{
	JSON_Value* value = json_object_get_value(config_obj, name);

	if (value && json_value_get_type(value) == JSONNumber)
		return (float)json_value_get_number(value);
	
	return default;
}

const char* Config_Json::GetString(const char* name, const char* default) const 
{
	JSON_Value* value = json_object_get_value(config_obj, name);

	if (value && json_value_get_type(value) == JSONString)
		return json_value_get_string(value);
	
	return default;
}

bool Config_Json::GetBool(const char* name, bool default) const 
{
	JSON_Value* value = json_object_get_value(config_obj, name);

	if (value && json_value_get_type(value) == JSONBoolean)
		return (bool)json_value_get_boolean;

	return default;	
}

bool Config_Json::SetInt(const char * name, int value)
{
	return json_object_set_number(config_obj, name, value) == JSONSuccess;
}

bool Config_Json::SetFloat(const char * name, float value)
{
	return json_object_set_number(config_obj, name, (float)value) == JSONSuccess;
}

bool Config_Json::SetString(const char * name, const char * value)
{
	return json_object_set_string(config_obj, name, value) == JSONSuccess;
}

bool Config_Json::SetBool(const char * name, bool value)
{
	return json_object_set_boolean(config_obj, name, value) == JSONSuccess;
}

void Config_Json::SaveToFile(const char* filename)
{
	json_serialize_to_file(config_val, filename);

	if (config_obj)
	{
		json_object_clear(config_obj);
		delete config_obj;
	}
}