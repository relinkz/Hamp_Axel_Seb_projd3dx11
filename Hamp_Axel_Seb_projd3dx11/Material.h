#ifndef MATERIAL_H
#define MATERIAL_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"
#include <string>

using namespace DirectX::SimpleMath;
using namespace std;

class Material
{
public:
	Material();
	Material(string materialName, Vector3 ambientColor, Vector3 diffuse, Vector3 specular);
	virtual ~Material();

	//getter 
	string getName() const;
	Vector3 getAmbient() const;
	Vector3 getDiffuse() const;
	Vector3 getSpecular() const;
	//setters
	void setName(const string& name);
	void setAmbient(const Vector3& newAmb);
	void setDiffuse(const Vector3& newDiffuse);
	void setSpecular(const Vector3& newSpec);
	void setTransmissionFilter(const Vector3& newTransmissionFilter);
	void setOpticalDensity(float opticalDensity);
	void setIlluminationModel(int newIlluminationModel);
private:
	string materialName;
	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;

	Vector3 transmissionFilter;
	float opticalDensity;
	unsigned int IlluminationModel;
};

#endif