#ifndef OBJECT_H
#define OBJECT_H
#include "Parser.h"


class Object
{
private:
	Vector3					position;
	Vector2					UV_Coord;
	vector<TriangleVertex>	vertexData;
	ID3D11Buffer*			vertexBuffer;

	/*
	Variables for the texture
	*/
	ID3D11Resource*				textureData;
	ID3D11ShaderResourceView*	diffuseMapSRV;
	ID3D11Texture2D*			diffuseMap;

	short int vertexSize;
	void createVertexBuffer(ID3D11Device* gDevice);
	void create2DTexture(ID3D11Device* gDevice, string srcImage);
public:
	Object();
	Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice, string srcImage);
	Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice);
	virtual ~Object();

	//getters setters
	Matrix getWorldMatrix()							const;
	ID3D11Buffer* getVertexBufferPointer()			const;
	ID3D11ShaderResourceView* getDiffuseMapSRV()	const;
	Vector3 getPosition()							const;
};

#endif