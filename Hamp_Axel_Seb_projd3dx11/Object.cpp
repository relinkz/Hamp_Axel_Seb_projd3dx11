#include "Object.h"
#include "WICTextureLoader.h"

Object::Object()
{
	this->position		= Vector3(0.0f, 0.0f, 0.0f);
	this->UV_Coord		= Vector2(0.0f, 0.0f);
	
	this->vertexBuffer	= nullptr;
	this->textureData	= nullptr;
	this->diffuseMap = nullptr;
	this->diffuseMapSRV = nullptr;
}

Object::Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice, string srcImage)
{
	this->vertexSize = fromParser.size();

	for (int i = 0; i < this->vertexSize; i++)
	{
		this->vertexData.push_back(fromParser.at(i));
	}
	this->position = worldPos;

	this->position = worldPos;

	this->vertexBuffer = nullptr;
	this->textureData = nullptr;
	this->diffuseMap = nullptr;
	this->diffuseMapSRV = nullptr;

	this->createVertexBuffer(gDevice);
	this->create2DTexture(gDevice, srcImage);
}

Object::Object(vector<TriangleVertex> fromParser, Vector3 worldPos, ID3D11Device * gDevice, string srcImage, string normalMap, int objID)
{
	
	this->vertexSize = fromParser.size();
	this->objID = objID;

	for (int i = 0; i < this->vertexSize; i++)
	{
		TriangleVertex temp = fromParser.at(i);
		temp.ID = objID;
		this->vertexData.push_back(temp);
		
		
		shadowtriVertex shadowVertexData;

		shadowVertexData.x = fromParser.at(i).x;
		shadowVertexData.y = fromParser.at(i).y;
		shadowVertexData.z = fromParser.at(i).z;

		this->shadowVertexData.push_back(shadowVertexData);
	}
	this->position = worldPos;

	this->position = worldPos;

	this->vertexBuffer = nullptr;
	this->textureData = nullptr;
	this->diffuseMap = nullptr;
	this->diffuseMapSRV = nullptr;
	this->normalMapData = nullptr;
	this->normalMapSRV = nullptr;

	this->createVertexBuffer(gDevice);
	this->createShadowVertexBuffer(gDevice);

	
	this->create2DTexture(gDevice, srcImage);
	this->createNoarmalMap(gDevice, normalMap);
}

Object::Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice)
{
	this->vertexSize = fromParser.size();

	for (int i = 0; i < this->vertexSize; i++)
	{
		this->vertexData.push_back(fromParser.at(i));
	}
	this->position = worldPos;

	this->position = worldPos;

	this->vertexBuffer = nullptr;
	this->textureData = nullptr;
	this->diffuseMap = nullptr;
	this->diffuseMapSRV = nullptr;

	this->createVertexBuffer(gDevice);
}

Object::~Object()
{

}

Matrix Object::getWorldMatrix() const
{
	Matrix test;

	test = Matrix(DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z));
	return test;
}

ID3D11Buffer* Object::getVertexBufferPointer() const
{
	return this->vertexBuffer;
}

ID3D11Buffer * Object::getShadowVertexBufferPointer() const
{
	return this->shadowVertexBuffer;
}

ID3D11ShaderResourceView* Object::getDiffuseMapSRV() const
{
	return this->diffuseMapSRV;
}

Vector3 Object::getPosition() const
{
	return this->position;
}

void Object::setPosY(float newY)
{
	this->position.y = newY;
}

ID3D11ShaderResourceView * Object::getNormalMapSRV() const
{
	return this->normalMapSRV;
}



#pragma region
/*
* Help and utility functions
*/

void Object::createVertexBuffer(ID3D11Device* gDevice)
{
	HRESULT result;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(*triangleVertices.data()) * nrOfVertexDrawn;  //triangleVertices
	bufferDesc.ByteWidth = sizeof(*this->vertexData.data()) * this->vertexSize;  //triangleVertices

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = this->vertexData.data();
	
	result = gDevice->CreateBuffer(&bufferDesc, &data, &this->vertexBuffer);
}

void Object::createShadowVertexBuffer(ID3D11Device* gDevice)
{
	HRESULT result;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(*triangleVertices.data()) * nrOfVertexDrawn;  //triangleVertices
	bufferDesc.ByteWidth = sizeof(*this->shadowVertexData.data()) * this->vertexSize;  //triangleVertices

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = this->shadowVertexData.data();

	result = gDevice->CreateBuffer(&bufferDesc, &data, &this->shadowVertexBuffer);
}

void Object::create2DTexture(ID3D11Device* gDevice, string fileName)
{
	CoInitialize(nullptr); 	//Need to call this function to be able to use the DirectX ToolKit!

	HRESULT result;

	//string fileName = "cube_box2.png";
	size_t maxsize = 0;

	wstring superString;
	for (int i = 0; i < fileName.length(); ++i)
		superString += wchar_t(fileName[i]);

	const wchar_t* hipsterFileTypeFileName = superString.c_str();

	result = DirectX::CreateWICTextureFromFileEx
		(
			gDevice,
			hipsterFileTypeFileName,
			maxsize,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			false,
			&this->textureData,
			&this->diffuseMapSRV
		);
}

void Object::createNoarmalMap(ID3D11Device * gDevice, string fileName)
{
	CoInitialize(nullptr); 	//Need to call this function to be able to use the DirectX ToolKit!

	HRESULT result;

	//string fileName = "brickNormalMap.png";
	size_t maxsize = 0;

	wstring superString;
	for (int i = 0; i < fileName.length(); ++i)
		superString += wchar_t(fileName[i]);

	const wchar_t* hipsterFileTypeFileName = superString.c_str();

	result = DirectX::CreateWICTextureFromFileEx
		(
			gDevice,
			hipsterFileTypeFileName,
			maxsize,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			false,
			&this->normalMapData,
			&this->normalMapSRV
			);
}

void Object::createObjectIndex(ID3D11Device * gDevice, int & Idnr)
{
	D3D11_TEXTURE2D_DESC descDepth2;
	descDepth2.Width = 640.0f;
	descDepth2.Height = 480.0f;
	descDepth2.MipLevels = 1;
	descDepth2.ArraySize = 1;
	descDepth2.Format = DXGI_FORMAT_R32_UINT;
	descDepth2.SampleDesc.Count = 4;
	descDepth2.SampleDesc.Quality = 0;
	descDepth2.Usage = D3D11_USAGE_DEFAULT;
	descDepth2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //must be used as RenderTargetViews and ShaderResources
	descDepth2.CPUAccessFlags = 0;
	descDepth2.MiscFlags = 0;


	HRESULT hr = gDevice->CreateTexture2D(&descDepth2, nullptr, &this->objectIndexTexture);
}

#pragma endregion