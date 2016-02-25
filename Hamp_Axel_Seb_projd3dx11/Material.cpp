#include "Material.h"

Material::Material()
{
	this->materialName = "Unknown";

	this->ambient = Vector3(0.0f, 0.0f, 0.0f);
	this->diffuse = Vector3(0.0f, 0.0f, 0.0f);
	this->specular = Vector3(0.0f, 0.0f, 0.0f);

	this->transmissionFilter = Vector3(0.0f, 0.0f, 0.0f);
	this->opticalDensity = 0;
	this->IlluminationModel = 0;
}

Material::Material(string materialName, Vector3 ambientColor, Vector3 diffuse, Vector3 specular)
{
	this->materialName	= materialName;
	this->ambient		= ambientColor;
	this->diffuse		= diffuse;
	this->specular		= specular;
}

Material::~Material()
{
}

string Material::getName() const
{
	return this->materialName;
}

Vector3 Material::getAmbient() const
{
	return this->ambient;
}

Vector3 Material::getDiffuse() const
{
	return this->diffuse;
}

Vector3 Material::getSpecular() const
{
	return this->specular;
}

void Material::setName(const string& name)
{
	this->materialName = name;
}

void Material::setAmbient(const Vector3& newAmb)
{
	this->ambient = newAmb;
}

void Material::setDiffuse(const Vector3& newDiffuse)
{
	this->diffuse = newDiffuse;
}

void Material::setSpecular(const Vector3& newSpec)
{
	this->specular = newSpec;
}

void Material::setTransmissionFilter(const Vector3& newTransmissionFilter)
{
	this->transmissionFilter = newTransmissionFilter;
}

void Material::setOpticalDensity(float opticalDensity)
{
	this->opticalDensity;
}

void Material::setIlluminationModel(int newIlluminationModel)
{
	this->IlluminationModel = newIlluminationModel;
}