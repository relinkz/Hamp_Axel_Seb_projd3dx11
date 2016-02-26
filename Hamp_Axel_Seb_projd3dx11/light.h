#ifndef LIGHT_H
#define LIGHT_H


#include <d3dcompiler.h>
#include "SimpleMath.h"
#include <d3d11.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


struct PosLight
{
	PosLight()
	{
		memset(this, 0, sizeof(this));
		this->x = 3.0f;
		this->y = 3.0f;
		this->z = 3.0f;
		this->w = 1.0f;
		this->pad = 0.0f;
		this->ambient = 0.2f;
		this->diffuse = 0.8f;
		this->strength = 5.0f;

	}
	PosLight(float x, float y, float z)
	{
		memset(this, 0, sizeof(this));
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = 1.0f;
		this->pad = 0.0f;
		this->ambient = 0.2f;
		this->diffuse = 0.8f;
		this->strength = 5.0f;

	}

	float x, y, z, w;
	float pad;
	float ambient;
	float diffuse;
	float strength;


};

//not yet used
struct DirLight
{
	DirLight()
	{
		memset(this, 0, sizeof(this));
		this->strength = 1.0f;
		this->x = 0;
		this->y = 0;
		this->z = 0;

	}
	float x, y, z;
	float strength;

};


#endif