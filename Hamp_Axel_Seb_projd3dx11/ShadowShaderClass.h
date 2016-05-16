#ifndef SHADOWSHADERCLASS_H
#define SHADOWSHADERCLASS_H


#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"
#include <fstream>
#include "Object.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


using namespace std;
using namespace DirectX::SimpleMath;
class ShadowShaderClass
{
private:
	struct MatrixBufferType
	{
		 Matrix world;
		 Matrix view;
		 Matrix proj;
		 Matrix lightView;
		 Matrix lightProjection;
	};

	struct LightBufferType
	{
		Vector4 ambientColor;
		Vector4 diffuseColor;
	};

	struct LightBufferType2
	{
		Vector3 lightPos;
		float padding;
	};

	ID3D11Texture2D* shadowmapDepthtexture;
	ID3D11DepthStencilView* shadowDepthStencilView;

	ID3D11VertexShader* shadow_vertexShader;
	ID3D11PixelShader* shadow_pixelShader;
	ID3D11InputLayout* shadow_layout;

	ID3D11SamplerState* shadow_sampleStateWrap;
	ID3D11SamplerState* shadow_sampleStateClamp;

	ID3D11Buffer* shadow_matrixBuffer;
	ID3D11Buffer* shadow_lightBuffer;
	ID3D11Buffer* shadow_lightBuffer2;

	bool initializeShader(ID3D11Device*, HWND hWind, WCHAR* vsfileName, WCHAR* psFileName);
	bool initializeDepthStencil(ID3D11Device* gDevice);
	void shutdownShader();
	void outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	bool setShaderParameters(
		ID3D11DeviceContext* gDeviceContext,
		Matrix worldMatrix,
		Matrix viewMatrix,
		Matrix projectionMatrix,
		Matrix lightViewMatrix,
		Matrix lightProjectionMatrix,
		ID3D11ShaderResourceView* texture,
		ID3D11ShaderResourceView* depthMapTexture,
		Vector3 lightPos,
		Vector4 ambientColor,
		Vector4 diffuseColor
		);
	void renderShader(ID3D11DeviceContext* gDeviceContext, Object toDraw);
	
public:
	ShadowShaderClass();
	virtual ~ShadowShaderClass();

	bool initialize(ID3D11Device*, HWND hWind);
	void shutdown();
	bool Render(
		ID3D11DeviceContext* gContextDevice, 
		int indexCount, 
		Matrix worldMatrix,
		Matrix viewMatrix,
		Matrix projectionMatrix,
		Matrix lightViewMatrix,
		Matrix lightProjectionMatrix,
		ID3D11ShaderResourceView* texture,
		ID3D11ShaderResourceView* depthMapTexture, 
		Vector3 lightPos,
		Vector4 ambientColor,
		Vector4 diffuseColor);
	//void renderShadowMap(Object toRender);
	ID3D11VertexShader* getShadowVS() const;
	ID3D11DepthStencilView* getDepthStencilView() const;
	ID3D11Texture2D* getDepthStencilRTV() const;
	ID3D11InputLayout* getInputLayout() const;




};


#endif

