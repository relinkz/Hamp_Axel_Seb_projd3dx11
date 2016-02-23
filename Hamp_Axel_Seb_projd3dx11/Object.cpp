#include "Object.h"

Object::Object()
{
	this->position		= Vector3(0.0f, 0.0f, 0.0f);
	this->UV_Coord		= Vector2(0.0f, 0.0f);
	
	this->VertexBuffer	= nullptr;
	this->TextureData	= nullptr;
}

Object::Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice)
{
	this->vertexSize = fromParser.size();

	for (int i = 0; i < this->vertexSize; i++)
	{
		this->vertexData.push_back(fromParser.at(i));
	}
	this->position = worldPos;

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
	return this->VertexBuffer;
}

#pragma region
/*
* Help and utility functions
*/

void Object::createVertexBuffer(ID3D11Device* gDevice)
{
	HRESULT tits;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(*triangleVertices.data()) * nrOfVertexDrawn;  //triangleVertices
	bufferDesc.ByteWidth = sizeof(*this->vertexData.data()) * this->vertexSize;  //triangleVertices

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = this->vertexData.data();
	
	tits = gDevice->CreateBuffer(&bufferDesc, &data, &this->VertexBuffer);
}

#pragma endregion