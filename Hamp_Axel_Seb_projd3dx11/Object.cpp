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
	//D3D11_TEXTURE2D_DESC image;
	//memset(&image, 0, sizeof(image));

	//image.Width = 1024;
	//image.Height = 1024;
	//image.MipLevels = 1;
	//image.ArraySize = 1;
	//image.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//image.SampleDesc.Count = 1;      //kolla upp
	//image.SampleDesc.Quality = 0;
	//image.Usage = D3D11_USAGE_DEFAULT;  //GPU behöver endast access
	//image.BindFlags = D3D11_BIND_SHADER_RESOURCE;  // har jag bindat den till någon resurs nu?
	//image.CPUAccessFlags = 0;
	//image.MiscFlags = 0;

	//D3D11_SUBRESOURCE_DATA imageData;
	//memset(&imageData, 0, sizeof(imageData));
	//imageData.pSysMem = this->textureData;
	//imageData.SysMemPitch = 1024 * 4 * sizeof(char);

	//D3D11_SHADER_RESOURCE_VIEW_DESC imageSRV;
	//memset(&imageSRV, 0, sizeof(imageSRV));
	//imageSRV.Format = image.Format;
	//imageSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//imageSRV.Texture2D.MipLevels = image.MipLevels;
	//imageSRV.Texture2D.MostDetailedMip = 0;
	//ID3D11Texture2D* temp = this->diffuseMap;

	//result = gDevice->CreateTexture2D(&image, &imageData, &temp);

	////result = gDevice->CreateShaderResourceView(this->diffuseMap, &imageSRV, &(this->diffuseMapSRV));

}

#pragma endregion