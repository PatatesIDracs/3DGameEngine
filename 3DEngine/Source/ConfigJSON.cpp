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
