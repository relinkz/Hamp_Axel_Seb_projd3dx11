#ifndef OBJECT_H
#define OBJECT_H
#include "Parser.h"


struct shadowtriVertex
{
	float x, y, z;
};
class Object
{
private:
	Vector3					position;
	Vector2					UV_Coord;
	vector<TriangleVertex>	vertexData;
	vector<shadowtriVertex> shadowVertexData;

	ID3D11Buffer*			vertexBuffer;
	ID3D11Buffer*			shadowVertexBuffer;

	/*
	Variables for the texture
	*/
	ID3D11Resource*				textureData;
	ID3D11ShaderResourceView*	diffuseMapSRV;
	ID3D11Texture2D*			diffuseMap;

	ID3D11Resource*				normalMapData;
	ID3D11ShaderResourceView*	normalMapSRV;

	short int vertexSize;
	void createVertexBuffer(ID3D11Device* gDevice);
	void createShadowVertexBuffer(ID3D11Device* gDevice);
	void create2DTexture(ID3D11Device* gDevice, string srcImage);
	void createNoarmalMap(ID3D11Device* gDevice, string fileName);
public:
	Object();
	Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice, string srcImage);
	Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice, string srcImage, string normalMap);
	Object(vector<TriangleVertex>fromParser, Vector3 worldPos, ID3D11Device* gDevice);
	Object(Vector3 worldPos, ID3D11ShaderResourceView* diffuseMapSRV, ID3D11ShaderResourceView* normalMapSRV, ID3D11Buffer* vertexBuffer, ID3D11Buffer* shadowVertexBuffer);
	virtual ~Object();

	//getters setters
	Matrix getWorldMatrix()							const;

	ID3D11Buffer* getVertexBufferPointer()			const;
	ID3D11Buffer* getShadowVertexBufferPointer()	const;

	ID3D11ShaderResourceView* getDiffuseMapSRV()	const;
	ID3D11ShaderResourceView* getNormalMapSRV()		const;
	Vector3 getPosition()							const;
};

#endif