#ifndef MAP_H
#define MAP_H
#include "Object.h"
#include "Camera.h"


struct worldMatrixBuffer
{
	XMFLOAT4X4 worldViewProj;
	XMFLOAT4X4 eyeSpace;
	XMFLOAT4X4 lightViewMatrix;
	XMFLOAT4X4 lightProjectionMatrix;
};
struct Node
{
	Node* parent;
	bool cLeft;
	bool cUp;
	Node()
	{
		this->parent = nullptr;
		this->cLeft = false;
		this->cUp = false;
	}
};
//const int WIDTH = 50;
//const int HEIGHT = 50;
class Map
{
private:
	//Node nodes[WIDTH][HEIGHT];
	//Object* wallPtr;
	//int nrOfGroups;
	SimpleMath::Matrix ProjectionMatrix;

	void MakeSet(Node* node);
	Node* FindSet(Node* node);
	bool Union(Node* node1, Node* node2);
	void renderWall(ID3D11DeviceContext* gDeviceContext, Camera* camera, ID3D11Buffer* worldSpaceBuffer,Vector3 offSet, Vector3 scale);
	void updateWorldSpaceBuffer(ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* worldSpaceBuffer, Matrix* worldSpace, Matrix* WVP);
	void updateSRV(ID3D11DeviceContext* gDeviceContext);
public:
	Map();
	Map(Object* wallPtr);
	~Map();

	void Render(ID3D11DeviceContext* gDeviceContext, Camera* camera, ID3D11Buffer* worldSpaceBuffer);

	void makeLabyrinth(Node** nodes, int width, int height);
	bool addConnection(int x, int y);
	bool makeConnection(Node** nodes, int x, int y, int direction);

};


#endif