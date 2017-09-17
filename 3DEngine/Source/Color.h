
#pragma once

struct Color
{
	float r, g, b, a;
	
	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{}

	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
	{}

	void Set(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & ()
	{
		return (float*)this;
	}
};

extern Color Red;
extern Color Orange;
extern Color Gold;
extern Color Lime  ;
extern Color Green ;
extern Color LightMediumSeaGreeen;
extern Color Cyan  ;
extern Color Blue  ;
extern Color DarkBlue;
extern Color Violet;
extern Color Purple ;
extern Color Magenta ;
extern Color SpringGreen;
extern Color Black ;
extern Color Grey ;
extern Color White ;
extern Color Brown;
extern Color LightColor;
extern Color Indigo;
extern Color Glass;
extern Color Invisible;
