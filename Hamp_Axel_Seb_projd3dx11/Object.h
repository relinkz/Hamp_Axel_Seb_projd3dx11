#ifndef OBJECT_H
#define OBJECT_H
#include "Parser.h"


class Object
{
private:
	Vector3					position;
	Vector2					UV_Coord;
	vector<TriangleVertex>	vertexData;
	ID3D11Buffer*			VertexBuffer;
	ID3D11Texture2D*		TextureData;

	short int vertexSize;
	void createVertexBuffer(ID3D11Device* gDevice);
public:
	Object();
	Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice);
	virtual ~Object();

	//getters setters
	Matrix getWorldMatrix() const;
	ID3D11Buffer* getVertexBufferPointer() const;

};

#endif