#include "ShadowShaderClass.h"

ShadowShaderClass::ShadowShaderClass()
{
	this->shadow_vertexShader		= nullptr;
	this->shadow_pixelShader		= nullptr;
	this->shadow_layout				= nullptr;
	this->shadow_matrixBuffer		= nullptr;
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

bool ShadowShaderClass::initialize(ID3D11Device* gDevice, HWND hWind, Matrix toConstantBuffer)
{
	bool result;
	//högst sannoligt fel här
	result = this->initializeShader(gDevice, hWind, L"shadowVS.hlsl", L"shadowPS.hlsl"); //restertek.com

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

void ShadowShaderClass::clearRenderTargetView(ID3D11DeviceContext* gDeviceContext)
{
	float clearColor[] = { 0, 0, 0, 1 };
	float whiteColor[] = { 1, 1, 1, 1 };
	float grayColor[] = { 0.5, 0.5, 0.5 , 1 };

	gDeviceContext->ClearRenderTargetView(this->shadowMapRTV, whiteColor);
}

void ShadowShaderClass::clearDepthBuffer(ID3D11DeviceContext* gDeviceContext)
{
	gDeviceContext->ClearDepthStencilView(this->shadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

bool ShadowShaderClass::initializeShader(ID3D11Device* gDevice, HWND hWind, WCHAR* vsFileName, WCHAR* psFileName)
{
	bool result = true;

	//creating shaders
	result = this->createVertexShader(gDevice, hWind, vsFileName);
	
	result = this->createPixelShader(gDevice, hWind, psFileName);

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
	textureDepthDesc.Width = 640.0f;
	textureDepthDesc.Height = 480.0f;
	textureDepthDesc.MipLevels = 1;
	textureDepthDesc.ArraySize = 1;
	textureDepthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	textureDepthDesc.SampleDesc.Count = 4;  //must match the swap chain
	textureDepthDesc.SampleDesc.Quality = 0;
	
	textureDepthDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDepthDesc.CPUAccessFlags = 0;
	textureDepthDesc.MiscFlags = 0;


	//create the texture for depthmap?
	resultHandler = gDevice->CreateTexture2D(
		&textureDepthDesc,
		NULL,
		&(this->shadowmapDepthtexture)
		);

	//create another texture for the render target view
	D3D11_TEXTURE2D_DESC textureDescRTV;

	ZeroMemory(&textureDescRTV, sizeof(textureDescRTV));

	//setup the render target texture description
	textureDescRTV.Width = 640.0f;
	textureDescRTV.Height = 480.0f;
	textureDescRTV.MipLevels = 1;
	textureDescRTV.ArraySize = 1;
	textureDescRTV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	textureDescRTV.SampleDesc.Count = 4;  //must match swapchain
	textureDescRTV.SampleDesc.Quality = 0;

	textureDescRTV.Usage = D3D11_USAGE_DEFAULT;
	textureDescRTV.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescRTV.CPUAccessFlags = 0;
	textureDescRTV.MiscFlags = 0;

	resultHandler = gDevice->CreateTexture2D(&textureDescRTV, NULL, &this->shadowmapRenderTargetViewTexture);



	//create render target view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourseViewDesc;
	
	ZeroMemory(&shaderResourseViewDesc, sizeof(shaderResourseViewDesc));

	//texture already created

	//setup the description of the render target view
	//didn't work, but with null it does, no questions asked

	//Create the render target view using the depthbuffer?
	//resultHandler = gDevice->CreateRenderTargetView(this->shadowmapRenderTargetViewTexture, &renderTargetViewDesc, &this->shadowMapRTV);
	//http://www.rastertek.com/dx11tut22.html <--- here
	resultHandler = gDevice->CreateRenderTargetView(this->shadowmapRenderTargetViewTexture, NULL, &this->shadowMapRTV);


	//set upp description to the shader resource view
	shaderResourseViewDesc.Format = textureDescRTV.Format;
	shaderResourseViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourseViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourseViewDesc.Texture2D.MipLevels = 0;

	//create the shader resourse view
	//resultHandler = gDevice->CreateShaderResourceView(this->shadowmapRenderTargetViewTexture, &shaderResourseViewDesc, &this->shaderResourceView);
	resultHandler = gDevice->CreateShaderResourceView(this->shadowmapRenderTargetViewTexture, NULL, &this->shaderResourceView);

	//create depthStencilView
	resultHandler = gDevice->CreateDepthStencilView(
		this->shadowmapDepthtexture,   //resorse we want to create a view to
		0,
		&(this->shadowDepthStencilView)  //the depthStencilView
		);

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
