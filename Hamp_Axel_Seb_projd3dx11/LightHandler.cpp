#include "LightHandler.h"

LightHandler::LightHandler()
{
	this->lights = new PosLight[16];
	this->nrOfLights = 0;

	memset(&this->lightDesc, 0, sizeof(this->lightDesc));
	this->lightDesc.Usage = D3D11_USAGE_DYNAMIC;
	this->lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	this->lightDesc.ByteWidth = sizeof(*lights);
	this->lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	this->lightDesc.MiscFlags = 0;
	this->lightDesc.StructureByteStride = 0;

	memset(&this->lightData, 0, sizeof(this->lightData));
}

LightHandler::~LightHandler()
{
	delete[]this->lights;
}

//funkar ej
bool LightHandler::sendToBuffer(ID3D11Device* device)
{
	if (device != nullptr)
	{
		this->lightData.pSysMem = lights;
		device->CreateBuffer(&lightDesc, &lightData, &this->cLightBuffer);

		if (this->cLightBuffer != nullptr)
		{
			return true;
		}
	}
	return false;
}

void LightHandler::addPosLight(float x, float y, float z)
{

	this->lights[this->nrOfLights++] = PosLight(x, y, z);
	this->lightDesc.ByteWidth = sizeof(*lights)*this->nrOfLights;
}

PosLight LightHandler::getPosLight(int index)
{
	return this->lights[index];
}

int LightHandler::getNrOfLights()
{
	return this->nrOfLights;
	//return this->lights.size();
}

D3D11_BUFFER_DESC LightHandler::getDesc()
{
	return this->lightDesc;
}

D3D11_SUBRESOURCE_DATA LightHandler::getData()
{
	return this->lightData;
}

PosLight* LightHandler::getLights()
{
	return this->lights;
}

ID3D11Buffer* LightHandler::getCLightBuffer() const
{
	return this->cLightBuffer;
}
