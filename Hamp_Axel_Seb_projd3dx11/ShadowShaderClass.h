#ifndef SHADOWSHADERCLASS_H
#define SHADOWSHADERCLASS_H


#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"
#include <fstream>
#include "Object.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>


#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

//set shadowmaps resulution here
float const SM_WIDTH = 640.0;
float const SM_HEIGHT = 480.0;

using namespace std;
class ShadowShaderClass
{
private:
	struct shadowMapMatrtixBuff
	{
		Matrix lightWorldViewProj;
		Matrix objWorldMatrix;
	};
	Vector3 lightPos;

	ID3D11Texture2D* shadowmapDepthtexture;
	ID3D11Texture2D* shadowmapRenderTargetViewTexture;
	ID3D11DepthStencilView* shadowDepthStencilView;

	ID3D11VertexShader* shadow_vertexShader;
	ID3D11PixelShader* shadow_pixelShader;
	ID3D11InputLayout* shadow_layout;

	//constant buffer for the shadowMap
	ID3D11Buffer* shadow_constantBuffer;

	//render target view
	ID3D11RenderTargetView* shadowMapRTV;
	ID3D11ShaderResourceView* shaderResourceView;

	Matrix worldMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	Matrix wvpMatrix;

	bool initializeShader(ID3D11Device*, HWND hWind, WCHAR* vsfileName, WCHAR* psFileName);
	bool initializeDepthStencil(ID3D11Device* gDevice);
	void shutdownShader();
	void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

	bool createVertexShader(ID3D11Device* gDevice, HWND hWind, WCHAR* vsFileName);
	bool createPixelShader(ID3D11Device* gDevixe, HWND hWind,WCHAR* psFileName); //helpshader

	bool createConstantBuffer(ID3D11Device* gDevice);
public:
	ShadowShaderClass();
	virtual ~ShadowShaderClass();

	bool initialize(ID3D11Device*, HWND hWind, Vector3 lightPos, Vector3 lookAt, Vector3 upVector) throw(...);
	void shutdown();

	//getters
	ID3D11VertexShader* getShadowVS() const;
	ID3D11PixelShader* getShadowPS() const;
	ID3D11Buffer* getLightBuffer() const;
	Vector3 getLightPos() const;

	ID3D11DepthStencilView* getDepthStencilView() const;
	ID3D11Texture2D* getDepthStencilRTV() const;
	ID3D11InputLayout* getInputLayout() const;
	ID3D11ShaderResourceView* getShaderResourceView() const;
	ID3D11RenderTargetView* getRenderTargetView() const;
	void render(Object &toDraw, int nrOfVertex,ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);


	//setters

	//void clearRenderTargetView(ID3D11DeviceContext* gDeviceContext);
	void clearDepthBuffer(ID3D11DeviceContext* gDeviceContext);
	void updateBuffer(Object &toDraw, ID3D11DeviceContext* gDeviceContext);
};


#endif

