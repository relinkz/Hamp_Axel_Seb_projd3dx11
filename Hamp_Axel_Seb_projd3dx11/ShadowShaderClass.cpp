#include "ShadowShaderClass.h"

ShadowShaderClass::ShadowShaderClass()
{
	this->shadow_vertexShader		= nullptr;
	this->shadow_pixelShader		= nullptr;
	this->shadow_layout				= nullptr;
	this->shadow_constantBuffer		= nullptr;

	this->shadowDepthStencilView	= nullptr;
	this->shadowmapDepthtexture		= nullptr;
	this->shadowMapRTV				= nullptr;
	this->shadowmapRenderTargetViewTexture = nullptr;
}

ShadowShaderClass::~ShadowShaderClass()
{
	this->shutdown();
}

bool ShadowShaderClass::initialize(ID3D11Device* gDevice, HWND hWind, Vector3 lightPos, Vector3 lookAt, Vector3 upVector) throw(...)
{
	bool result;
	HRESULT helper;

	//create depthBuffer
	result = this->initializeDepthStencil(gDevice);
	if (result == false)
	{
		throw("ShadowMap: Failed to initialize depthStencil");
	}
	//create vertex shader
	result = this->createVertexShader(gDevice, hWind, L"shadowVS.hlsl");
	if (result == false)
	{
		throw("ShadowMap: Failed to create vertexshader");
	}

	//creating matrices, identityMatrix
	this->worldMatrix = Matrix();
	//rotating the world in to the lights view
	this->lightPos = Vector3(0, 10, 0);
	this->lightPos = lightPos;

	this->viewMatrix = Matrix(DirectX::XMMatrixLookAtLH(
		this->lightPos,	    //lights position
		Vector3(1,-1,0),	//Look at target
		Vector3(0,1,0)		//Upvector
		));
	//adding projection

	this->projectionMatrix = Matrix(DirectX::XMMatrixPerspectiveFovLH
		(
			3.14f*0.45f,		// FOV
			640.0f / 480.0f,	//Aspect Ratio
			0.5f,				//near plane
			20.0f				//far plane
			));

	//this->projectionMatrix = Matrix(DirectX::XMMatrixOrthographicLH(640.0f, 480.f, 0.5f, 20.0f));


	//Matrix multipication to worldViewProjection
	this->wvpMatrix = (this->worldMatrix * this->viewMatrix) * this->projectionMatrix;
	//transpose it for the pipeline
	this->wvpMatrix = this->wvpMatrix.Transpose();

	//creating the constant buffer
	result = this->createConstantBuffer(gDevice);
	if (result == false)
	{
		throw("ShadowMap: Failed creating the constantbuffer");
	}

	return result;
}

void ShadowShaderClass::shutdown()
{
	this->shutdownShader();
}

ID3D11VertexShader* ShadowShaderClass::getShadowVS() const
{
	return this->shadow_vertexShader;
}

ID3D11PixelShader * ShadowShaderClass::getShadowPS() const
{
	return this->shadow_pixelShader;
}

ID3D11Buffer* ShadowShaderClass::getLightBuffer() const
{
	return this->shadow_constantBuffer;
}

Vector3 ShadowShaderClass::getLightPos() const
{
	return this->lightPos;
}

ID3D11DepthStencilView* ShadowShaderClass::getDepthStencilView() const
{
	return this->shadowDepthStencilView;
}

ID3D11Texture2D* ShadowShaderClass::getDepthStencilRTV() const
{
	return this->shadowmapDepthtexture;
}

ID3D11InputLayout* ShadowShaderClass::getInputLayout() const
{
	return this->shadow_layout;
}

ID3D11ShaderResourceView* ShadowShaderClass::getShaderResourceView() const
{
	return this->shaderResourceView;
}

ID3D11RenderTargetView* ShadowShaderClass::getRenderTargetView() const
{
	return this->shadowMapRTV;
}

void ShadowShaderClass::render(Object & toDraw, int nrOfVertex, ID3D11Device * gDevice, ID3D11DeviceContext* gDeviceContext)
{
	UINT32 vertexSize = sizeof(shadowtriVertex);
	UINT32 offset = 0;

	ID3D11Buffer* vertexBuffer = nullptr;

	vertexBuffer = toDraw.getShadowVertexBufferPointer();

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(this->shadow_layout);

	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	gDeviceContext->OMSetRenderTargets(0, nullptr, this->shadowDepthStencilView);
	//gDeviceContext->OMSetRenderTargets(2, &shadowBufferRTV, gDepthBuffer);


	gDeviceContext->VSSetShader(this->shadow_vertexShader, NULL, 0);
	gDeviceContext->GSSetShader(NULL, NULL, 0);
	gDeviceContext->PSSetShader(NULL, NULL, 0);

	UINT startslot = 0;
	UINT nrOfBuffers = 1;

	gDeviceContext->VSSetConstantBuffers(startslot, nrOfBuffers, &this->shadow_constantBuffer);

	gDeviceContext->Draw(nrOfVertex, 0);

}

void ShadowShaderClass::clearDepthBuffer(ID3D11DeviceContext* gDeviceContext)
{
	gDeviceContext->ClearDepthStencilView(this->shadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowShaderClass::updateBuffer(Object & toDraw, ID3D11DeviceContext* gDeviceContext)
{
	shadowMapMatrtixBuff* Light_Ptr = nullptr;
	
	Matrix objWorldMatrix = toDraw.getWorldMatrix().Transpose();

	shadowMapMatrtixBuff updatedBuffer
	{
		this->wvpMatrix,
		objWorldMatrix
	};

	D3D11_MAPPED_SUBRESOURCE lightViewSpaceData;
	memset(&lightViewSpaceData, 0, sizeof(lightViewSpaceData));

	HRESULT test = gDeviceContext->Map(this->shadow_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightViewSpaceData);
	memcpy(lightViewSpaceData.pData, &updatedBuffer, sizeof(updatedBuffer));
	gDeviceContext->Unmap(this->shadow_constantBuffer, 0);
}

bool ShadowShaderClass::initializeShader(ID3D11Device* gDevice, HWND hWind, WCHAR* vsFileName, WCHAR* psFileName)
{
	bool result = true;

	//creating shaders
	result = this->createVertexShader(gDevice, hWind, vsFileName);
	
	//creating the depth buffer
	if(this->initializeDepthStencil(gDevice) == false);
	{
		result = false;
	}
	return result;
}

bool ShadowShaderClass::initializeDepthStencil(ID3D11Device* gDevice)
{
	HRESULT resultHandler;
	bool result = false;

	D3D11_TEXTURE2D_DESC textureDepthDesc;
	ZeroMemory(&textureDepthDesc, sizeof(textureDepthDesc));
	textureDepthDesc.Width = 640.0f;
	textureDepthDesc.Height = 480.0f;
	textureDepthDesc.MipLevels = 1;
	textureDepthDesc.ArraySize = 1;
	textureDepthDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	textureDepthDesc.SampleDesc.Count = 1;  //must match the swap chain
	textureDepthDesc.SampleDesc.Quality = 0;
	
	textureDepthDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDepthDesc.CPUAccessFlags = 0;
	textureDepthDesc.MiscFlags = 0;

	//create the texture for depthmap?
	resultHandler = gDevice->CreateTexture2D(&textureDepthDesc,	NULL, &(this->shadowmapDepthtexture));

	//create Depth Stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	resultHandler = gDevice->CreateDepthStencilView(this->shadowmapDepthtexture, &dsvDesc, &(this->shadowDepthStencilView));


	//create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	//create the shader resourse view
	resultHandler = gDevice->CreateShaderResourceView(this->shadowmapDepthtexture, &srvDesc, &this->shaderResourceView);

	if (!FAILED(resultHandler))
	{
		result = true;
	}
	return result;
}

void ShadowShaderClass::shutdownShader()
{
	
	// Release the layout.
	if (this->shadow_layout)
	{
		this->shadow_layout->Release();
		this->shadow_layout = 0;
	}

	// Release the vertex shader.
	if (this->shadow_vertexShader)
	{
		this->shadow_vertexShader->Release();
		this->shadow_vertexShader = 0;
	}

	if (this->shadowmapRenderTargetViewTexture)
	{
		this->shadowmapRenderTargetViewTexture->Release();
		this->shadowmapRenderTargetViewTexture = nullptr;
	}
}

void ShadowShaderClass::outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ShadowShaderClass::createVertexShader(ID3D11Device* gDevice, HWND hWind,WCHAR* vsFileName)
{
	bool totalResult = true;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* errorMessage;
	HRESULT result;

	result = D3DCompileFromFile(
		vsFileName, //filename
		NULL,
		NULL,
		"ShadowVertexShader",
		"vs_4_0",
		D3DCOMPILE_DEBUG,  //D3D10_SHADER_ENABLE_STRICTNESS
		0,
		&vertexShaderBuffer,
		&errorMessage
		);

	if (FAILED(result))
	{
		this->outputShaderErrorMessage(errorMessage, hWind, vsFileName);
		MessageBox(hWind, vsFileName, L"Missing Shader File", MB_OK);
	}

	//creating the vertex shader from buffer
	result = gDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &(this->shadow_vertexShader));

	// Create the vertex input layout description
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	int numElements = 0;

	polygonLayout[0].SemanticName = "SHADOW_POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;


	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = gDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&(this->shadow_layout));

	if (FAILED(result))
	{
		//crash
		totalResult = false;


	}
	return totalResult;
}

bool ShadowShaderClass::createPixelShader(ID3D11Device* gDevice, HWND hWind,WCHAR* psFileName)
{
	bool result = true;

	bool totalResult = true;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	ID3D10Blob* errorMessage;
	HRESULT resultHelp;

	resultHelp = D3DCompileFromFile(
		psFileName, //filename
		NULL,
		NULL,
		"ShadowPixelShader",
		"ps_4_0",
		D3DCOMPILE_DEBUG,  //D3D10_SHADER_ENABLE_STRICTNESS
		0,
		&pixelShaderBuffer,
		&errorMessage
		);

	if (FAILED(resultHelp))
	{
		this->outputShaderErrorMessage(errorMessage, hWind, psFileName);
		MessageBox(hWind, psFileName, L"Missing Shader File", MB_OK);
	}

	resultHelp = gDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL ,&(this->shadow_pixelShader));

	return result;
}

bool ShadowShaderClass::createConstantBuffer(ID3D11Device * gDevice)
{
	shadowMapMatrtixBuff shadowCB
	{
		this->wvpMatrix,
		this->wvpMatrix
	};
	
	HRESULT help;
	bool result = true;

	//This will never change in the application
	D3D11_BUFFER_DESC constantBuffer;
	constantBuffer.ByteWidth = sizeof(shadowMapMatrtixBuff);
	constantBuffer.Usage = D3D11_USAGE_DYNAMIC;
	constantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBuffer.MiscFlags = NULL;
	constantBuffer.StructureByteStride = NULL;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &shadowCB;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	help = gDevice->CreateBuffer(&constantBuffer, &initData, &this->shadow_constantBuffer);
	
	if (help)
	{
		result = false;
	}
	
	return result;
}
