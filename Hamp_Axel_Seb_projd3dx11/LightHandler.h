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

public:
	LightHandler();
	virtual ~LightHandler();

	bool sendToBuffer(ID3D11Device* device, ID3D11Buffer* buffer);

	void addPosLight(float x = 0, float y = 0, float z = 0);
	
	PosLight getPosLight(int index);
	int getNrOfLights();
	D3D11_BUFFER_DESC getDesc();
	D3D11_SUBRESOURCE_DATA getData();
	PosLight* getLights();

};