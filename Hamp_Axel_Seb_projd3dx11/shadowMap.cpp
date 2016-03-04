#include "ShadowMap.h"



ShadowMap::ShadowMap()
{
	this->height = 0;
	this->width = 0;
	this->DepthMapSRV = nullptr;
	this->DepthMapDSV = nullptr;
}

ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height)
{
	HRESULT result;

	this->width = width;
	this->height = height;

	this->viewPort.TopLeftX = 0.0f;
	this->viewPort.TopLeftY = 0.0f;
	this->viewPort.Width = static_cast<float>(width);
	this->viewPort.Height = static_cast<float>(height);
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
	/*
	////////////////////////////////////
	//			VIKTIGT				  //
	////////////////////////////////////
	här är det viktigt att andvända "typeless" formatet eftersom depth stencil
	
	kommer att tolka bitarna som 
	"DXGI_FORMAT_D24_UNORM_S8_UINT", 
	
	däremot kommer
	ShaderResoursenViewn att tolka bitarna som 
	"DXGI_FORMAT_D24_UNORM_X8_TYPELESS"
	*/

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width				= this->width;
	texDesc.Height				= this->height;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.Format				= DXGI_FORMAT_R24G8_TYPELESS; //läs kommentar!!!
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;

	ID3D11Texture2D* depthMap = 0;
	result = device->CreateTexture2D(&texDesc, 0, &depthMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthMap, &dsvDesc, &this->DepthMapDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(depthMap, &srvDesc, &this->DepthMapSRV);

	//ReleaseCOM(depthMap);
}

ShadowMap::~ShadowMap()
{
}

ID3D11ShaderResourceView* ShadowMap::getDepthMapSRV() const
{
	return this->DepthMapSRV;
}

SimpleMath::Matrix ShadowMap::getOrtographProj() const
{
	SimpleMath::Matrix toReturn;

	toReturn = DirectX::XMMatrixOrthographicLH(this->width, this->height, 0.5f, 20.0f);

	return toReturn;
}

void ShadowMap::bindDsvandSetNullRenderTarget(ID3D11DeviceContext* deviceContect)
{
	deviceContect->RSSetViewports(1, &this->viewPort);

	/*
	*sätter null på render target render target eftersom vi endast kommer att måla till depth buffer.
	Om vi sätter null render target så kommer vi ta bort färg skrivningar.
	*/
	ID3D11RenderTargetView* renderTarget[1] = { 0 };
	deviceContect->OMSetRenderTargets(1, renderTarget, this->DepthMapDSV);
	deviceContect->ClearDepthStencilView(this->DepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}