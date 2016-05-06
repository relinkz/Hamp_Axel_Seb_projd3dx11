#include "ShadowShaderClass.h"

ShadowShaderClass::ShadowShaderClass()
{
	this->shadow_vertexShader		= nullptr;
	this->shadow_pixelShader		= nullptr;

	this->shadow_layout				= nullptr;
	this->shadow_sampleStateWrap	= nullptr;

	this->shadow_matrixBuffer		= nullptr;
	this->shadow_lightBuffer		= nullptr;
	this->shadow_lightBuffer2		= nullptr;

	this->shadowDepthStencilView = nullptr;
	this->shadowmapDepthtexture = nullptr;
}

ShadowShaderClass::~ShadowShaderClass()
{
}

bool ShadowShaderClass::initialize(ID3D11Device* gDevice, HWND hWind)
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

bool ShadowShaderClass::Render(
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
	Vector4 diffuseColor)
{
	bool result;

	result = this->setShaderParameters(
		gContextDevice, 
		worldMatrix, 
		viewMatrix, 
		projectionMatrix, 
		lightViewMatrix, 
		lightProjectionMatrix, 
		texture, 
		depthMapTexture, 
		lightPos, 
		ambientColor, 
		diffuseColor);

	if (result == true)
	{
		//renderShader(gContextDevice, indexCount);
	}

	return result;
}

ID3D11VertexShader* ShadowShaderClass::getShadowVS() const
{
	return this->shadow_vertexShader;
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

/*
Private functions
*/

bool ShadowShaderClass::initializeShader(ID3D11Device* gDevice, HWND hWind, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;
	bool resultBool;
	ID3D10Blob* errorMessage		= nullptr;
	ID3D10Blob* vertexShaderBuffer	= nullptr;
	ID3D10Blob* pixelShaderBuffer	= nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;

	//there is now a new description for the second clamp
	//sampler state used in the pixel shader

	//loading shaders!

	/*
	******************
	*	vertexShader *
	******************
	*/

	

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

		if(FAILED(result))
		{
			this->outputShaderErrorMessage(errorMessage, hWind, vsFileName);
			MessageBox(hWind, vsFileName, L"Missing Shader File", MB_OK);
		}
		// If there was nothing in the error message then it simply could not find the file itself.

/*
		/*
		******************
		*	PixelShader  *
		******************
		

		result = D3DCompileFromFile(
			vsFileName, //filename
			NULL,
			NULL,
			"ShadowPixelShader",
			"ps_4_0",
			D3DCOMPILE_DEBUG,  //D3D10_SHADER_ENABLE_STRICTNESS
			0,
			&pixelShaderBuffer,
			&errorMessage
			);

		if (FAILED(result))
		{
			this->outputShaderErrorMessage(errorMessage, hWind, psFileName);
		}
		else
		{
			MessageBox(hWind, psFileName, L"Missing Shader File", MB_OK);
		}

		*/

		//creating the vertex shader from buffer
		result = gDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &(this->shadow_vertexShader));

		if (FAILED(result))
		{
			//das crash
		}

		//result = gDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &(this->shadow_pixelShader));

		//if (FAILED(result))
		//{
		//	//das crash
		//}

		// Create the vertex input layout description
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		polygonLayout[2].SemanticName = "NORMAL";
		polygonLayout[2].SemanticIndex = 0;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[2].InputSlot = 0;
		polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[2].InstanceDataStepRate = 0;

		// Get a count of the elements in the layout.
		numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		// Create the vertex input layout.
		result = gDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
			&(this->shadow_layout));

		// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
		vertexShaderBuffer->Release();
		vertexShaderBuffer = 0;

		//pixelShaderBuffer->Release();
		//pixelShaderBuffer = 0;

		// Create a wrap texture sampler state description.
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Create the texture sampler state.
		result = gDevice->CreateSamplerState(&samplerDesc, &(this->shadow_sampleStateWrap));
		if (FAILED(result))
		{
			return false;
		}

		// Create a clamp texture sampler state description.
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		// Create the texture sampler state.
		result = gDevice->CreateSamplerState(&samplerDesc, &(this->shadow_sampleStateClamp));
		if (FAILED(result))
		{
			return false;
		}

		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = gDevice->CreateBuffer(&matrixBufferDesc, NULL, &(this->shadow_matrixBuffer));
		if (FAILED(result))
		{
			return false;
		}

		// Setup the description of the light dynamic constant buffer that is in the pixel shader.
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(LightBufferType);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = 0;
		lightBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
		result = gDevice->CreateBuffer(&lightBufferDesc, NULL, &(this->shadow_lightBuffer));
		if (FAILED(result))
		{
			return false;
		}

		// Setup the description of the light dynamic constant buffer that is in the vertex shader.
		lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
		lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc2.MiscFlags = 0;
		lightBufferDesc2.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = gDevice->CreateBuffer(&lightBufferDesc2, NULL, &(this->shadow_lightBuffer2));

		if(this->initializeDepthStencil(gDevice) == false);
		{
			//error depthStencil
			return false;
		}

		if (FAILED(result))
		{
			return false;
		}

		return true;
}

bool ShadowShaderClass::initializeDepthStencil(ID3D11Device* gDevice)
{
	HRESULT resultHandler;
	bool result = false;

	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = 640.0f;
	depthDesc.Height = 480.0f;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	depthDesc.SampleDesc.Count = 4;  //must match the swap chain
	depthDesc.SampleDesc.Quality = 0;

	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	//create the texture
	resultHandler = gDevice->CreateTexture2D(
		&depthDesc,
		0,
		&(this->shadowmapDepthtexture)
		);

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
	// Release the light constant buffers.
	if (this->shadow_lightBuffer)
	{
		this->shadow_lightBuffer->Release();
		this->shadow_lightBuffer = 0;
	}

	if (this->shadow_lightBuffer2)
	{
		this->shadow_lightBuffer2->Release();
		this->shadow_lightBuffer2 = 0;
	}

	// Release the matrix constant buffer.
	if (this->shadow_matrixBuffer)
	{
		this->shadow_matrixBuffer->Release();
		this->shadow_matrixBuffer = 0;
	}

	// Release the sampler states.
	if (this->shadow_sampleStateWrap)
	{
		this->shadow_sampleStateWrap->Release();
		this->shadow_sampleStateWrap = 0;
	}
	//release the new clamp sampler here
	if (this->shadow_sampleStateClamp)
	{
		this->shadow_sampleStateClamp->Release();
		this->shadow_sampleStateClamp = 0;
	}

	// Release the layout.
	if (this->shadow_layout)
	{
		this->shadow_layout->Release();
		this->shadow_layout = 0;
	}

	// Release the pixel shader.
	if (this->shadow_pixelShader)
	{
		this->shadow_pixelShader->Release();
		this->shadow_pixelShader = 0;
	}

	// Release the vertex shader.
	if (this->shadow_vertexShader)
	{
		this->shadow_vertexShader->Release();
		this->shadow_vertexShader = 0;
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

bool ShadowShaderClass::setShaderParameters(
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
	Vector4 diffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	LightBufferType2* dataPtr3;

	dataPtr = new MatrixBufferType;

	dataPtr->lightView = lightViewMatrix;
	dataPtr->lightProjection = lightProjectionMatrix;

	// Unlock the constant buffer.
	gDeviceContext->Unmap(this->shadow_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	gDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &(this->shadow_matrixBuffer));

	// Set shader texture resource in the pixel shader.
	gDeviceContext->PSSetShaderResources(0, 1, &texture);

	gDeviceContext->PSSetShaderResources(1, 1, &depthMapTexture);

	// Lock the light constant buffer so it can be written to.
	result = gDeviceContext->Map(this->shadow_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;

	// Unlock the constant buffer.
	gDeviceContext->Unmap(this->shadow_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	gDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &(this->shadow_lightBuffer));

	// Lock the second light constant buffer so it can be written to.
	result = gDeviceContext->Map(this->shadow_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (LightBufferType2*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr3->lightPos = lightPos;
	dataPtr3->padding = 0.0f;

	// Unlock the constant buffer.
	gDeviceContext->Unmap(this->shadow_lightBuffer2, 0);

	// Set the position of the light constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	gDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &(this->shadow_lightBuffer2));
}

void ShadowShaderClass::renderShader(ID3D11DeviceContext* gDeviceContext, Object toDraw)
{
	// Set the vertex input layout.
	gDeviceContext->IASetInputLayout(this->shadow_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	gDeviceContext->VSSetShader(this->shadow_vertexShader, NULL, 0);
	//gDeviceContext->PSSetShader(this->shadow_pixelShader, NULL, 0);

	// Set the sampler states in the pixel shader.
	//gDeviceContext->PSSetSamplers(0, 1, &(this->shadow_sampleStateClamp));
	//gDeviceContext->PSSetSamplers(1, 1, &(this->shadow_sampleStateWrap));

	//gDeviceContext->OMSetRenderTargets(1, &deferredViews[0], shadowMap.getDepthStencilView());

	// Render the triangle.
	//Render
	//gDeviceContext->Draw();

}