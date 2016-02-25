#pragma once
#include <Vector>
#include "light.h"

using namespace std;

class LightHandler
{
private:
	//vector<PosLight> lights;
	PosLight* lights;
	int nrOfLights;
	D3D11_BUFFER_DESC lightDesc;
	D3D11_SUBRESOURCE_DATA lightData;
	ID3D11Buffer* cLightBuffer;
public:
	LightHandler();
	virtual ~LightHandler();

	bool sendToBuffer(ID3D11Device* device);

	void addPosLight(float x = 0, float y = 0, float z = 0);
	
	PosLight getPosLight(int index);
	int getNrOfLights();
	ID3D11Buffer* getCLightBuffer() const;
	D3D11_BUFFER_DESC getDesc();
	D3D11_SUBRESOURCE_DATA getData();
	PosLight* getLights();

};